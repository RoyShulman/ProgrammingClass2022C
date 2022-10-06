import logging
from pathlib import Path
import threading
from datetime import datetime
from ipaddress import IPv4Address
from backup_server.connection_interface import AbstractConnectionInterface, Address, IncomingConnection
from backup_server.crc32 import crc32
from backup_server.protocol.client_message import (
    ClientMessageParser,
    ClientMessageReader,
    ClientMessageWithHeader,
    ClientMessageCode,
    ClientRegistrationRequest,
    ClientPublicKeyMessage,
    FileCRCIncorrectGivingUpMessage,
    FileCRCIncorrectWillRetryMessage,
    FileCRCOKMessage,
    UploadFileMessage,
)
from backup_server.protocol.server_message import (
    RegistrationSuccessfulMessage,
    AESKeyMessage,
    UploadFileSuccessfulMessage,
    SuccessResponseMessage
)
from backup_server.server_model import AbstractServerModel, Client
from backup_server.encryption_utils import AbstractEncryptionUtils
from backup_server.file_manager import FileManager


class ClientAlreadyRegisteredException(Exception):
    pass


class WrongMessageReceived(Exception):
    pass


class ClientConnectionMessageReader:
    def __init__(self, client_connection: AbstractConnectionInterface) -> None:
        self.client_connection = client_connection
        self.reader = ClientMessageReader(self.client_connection)

    def read_message(self) -> ClientMessageWithHeader:
        return ClientMessageParser.parse_message(self.reader)


class Server:
    NUM_LISTENING_CONNECTIONS = 10
    SERVER_VERSION = 3
    AES_KEY_SIZE = 16
    LISTENING_ADDRESS = IPv4Address("0.0.0.0")

    def __init__(self, port: int, connection: AbstractConnectionInterface, model: AbstractServerModel,
                 encryption_utils: AbstractEncryptionUtils,
                 base_storage_path: Path) -> None:
        self.port = port
        self.connection = connection
        self.model = model
        self.encryption_utils = encryption_utils
        self.file_manager = FileManager(base_storage_path)
        self.logger = logging.getLogger(__name__)

    def server_requests(self) -> None:
        self.logger.info(f"Starting to serve requests on port: {self.port}")
        self.connection.bind(Address(self.LISTENING_ADDRESS, self.port))
        self.connection.listen(self.NUM_LISTENING_CONNECTIONS)
        while True:
            incoming_connection = self.connection.accept()
            threading.Thread(target=self.handle_client_connection,
                             args=(incoming_connection, ))

    def handle_client_connection(self, incoming_connection: IncomingConnection) -> None:
        """
        Handle a new client connection.
        2 initial messages are possible:
            - Registration (If this is the first time the client is connecting)
            - Send public key (If the client is already registered)
        Afterwards we expect the following
            - An upload file request
            - A checksum verification message
        """
        self.logger.info(f"New connection from {incoming_connection.addr}")
        message_handler = ClientConnectionMessageReader(
            incoming_connection.connection)
        message = message_handler.read_message()
        was_registration = self.handle_possible_registration_request(incoming_connection.connection,
                                                                     message)
        if was_registration:
            # If it was a registration message, we must wait for the second public key message
            message = message_handler.read_message()
        self.handle_public_key_message(incoming_connection.connection, message)

        message = message_handler.read_message()
        self.handle_upload_file_message(incoming_connection.connection,
                                        message)
        # The upload file message can be sent again, or a checksum ok, or final checksum incorrect
        self.handle_post_file_upload(message_handler, incoming_connection)

    def handle_post_file_upload(self, message_handler: ClientConnectionMessageReader, incoming_connection: IncomingConnection):
        message = message_handler.read_message()
        while isinstance(message.payload, FileCRCIncorrectWillRetryMessage):
            message = message_handler.read_message()
            self.handle_upload_file_message(
                incoming_connection.connection, message)
            message = message_handler.read_message()
        # Client either accepted our checksum, or gave up
        if isinstance(message.payload, FileCRCIncorrectGivingUpMessage):
            self.logger.error(f"{message.header.client_id} Gave up")
        elif isinstance(message.payload, FileCRCOKMessage):
            incoming_connection.connection.send(
                SuccessResponseMessage(self.SERVER_VERSION).pack())
        else:
            raise WrongMessageReceived(message)
        self.logger.info(f"Closing connection from {incoming_connection.addr}")
        incoming_connection.connection.close()

    def handle_upload_file_message(self, client_connection: AbstractConnectionInterface,
                                   message: ClientMessageWithHeader):
        if not isinstance(message.payload, UploadFileMessage):
            raise WrongMessageReceived(message)

        self.logger.info(f"{message.header.client_id} uploaded a file")
        aes_key = self.model.get_client_aes_key(message.header.client_id)
        plain_content = self.encryption_utils.aes_decrypt(
            message.payload.content, aes_key)
        checksum = crc32(plain_content)
        local_path = self.file_manager.store_file(plain_content)

        self.model.store_file(message.header.client_id,
                              message.payload.filename, local_path)
        response = UploadFileSuccessfulMessage(self.SERVER_VERSION,
                                               message.header.client_id,
                                               len(plain_content), message.payload.filename, checksum)
        client_connection.send(response.pack())

    def handle_public_key_message(self, client_connection: AbstractConnectionInterface,
                                  message: ClientMessageWithHeader):
        if not isinstance(message.payload, ClientPublicKeyMessage):
            raise WrongMessageReceived(message)

        self.logger.info(f"Updating public key for {message.header.client_id}")
        aes_key = self.encryption_utils.get_aes_key(self.AES_KEY_SIZE)
        self.model.update_client_keys(message.header.client_id,
                                      message.payload.public_key,
                                      aes_key)
        encrypted_aes_key = self.encryption_utils.rsa_encrypt(
            aes_key, message.payload.public_key)
        response = AESKeyMessage(self.SERVER_VERSION,
                                 message.header.client_id,
                                 encrypted_aes_key)
        client_connection.send(response.pack())

    def handle_possible_registration_request(self, client_connection: AbstractConnectionInterface,
                                             initial_message: ClientMessageWithHeader) -> bool:
        """
        If the message is a registration request, register the client and return True.
        If it isn't, then we return False
        """
        if initial_message.header.code == ClientMessageCode.REGISTRATION:
            self.register_client(client_connection, initial_message)
            return True
        return False

    def register_client(self, client_connection: AbstractConnectionInterface, registration_message: ClientMessageWithHeader):
        self.logger.info(f"Trying to register new client")
        if not isinstance(registration_message.payload, ClientRegistrationRequest):
            raise WrongMessageReceived(registration_message)

        client_uuid = registration_message.header.client_id
        if self.model.is_client_registered(client_uuid):
            self.logger.error(f"Client: {client_uuid} is already registered")
            # TODO: send error
            raise ClientAlreadyRegisteredException(client_uuid)

        # We still don't know the public or aes key
        # TODO: is this correct?
        client = Client(client_uuid,
                        registration_message.payload.name,
                        b"",
                        datetime.now(),
                        b"")
        self.model.register_client(client)
        response = RegistrationSuccessfulMessage(self.SERVER_VERSION,
                                                 registration_message.header.client_id)
        client_connection.send(response.pack())

    # def update_client_encryption_keys(self, client_connection: AbstractConnectionInterface, public_key_message: ClientMessageWithHeader):

    def close(self):
        self.connection.close()
