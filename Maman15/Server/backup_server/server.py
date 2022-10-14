import uuid
import logging
import threading
from pathlib import Path
from datetime import datetime
from ipaddress import IPv4Address
from contextlib import closing
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
    RegistrationFailedMessage,
    RegistrationSuccessfulMessage,
    AESKeyMessage,
    UploadFileSuccessfulMessage,
    SuccessResponseMessage
)
from backup_server.server_model import AbstractServerModel, Client
from backup_server.abstract_encryption_utils import AbstractEncryptionUtils
from backup_server.file_manager import FileManager


class ClientAlreadyRegisteredException(Exception):
    pass


class WrongMessageReceived(Exception):
    pass


class FailedToRegisterClient(Exception):
    pass


class ClientConnectionMessageReader:
    """
    Helper class to read messages from an AbstractConnectionInterface.
    """

    def __init__(self, client_connection: AbstractConnectionInterface, model: AbstractServerModel) -> None:
        self.client_connection = client_connection
        self.model = model
        self.reader = ClientMessageReader(self.client_connection)

    def read_message(self) -> ClientMessageWithHeader:
        """
        Read a message and update the last seen time of the client
        """
        message = ClientMessageParser.parse_message(self.reader)
        self.model.update_last_seen_time(message.header.client_id)
        return message


class Server:
    """
    The server class. 
    Responsible for serving requests to clients and updating the databse
    """
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
                             args=(incoming_connection, ),
                             name=str(incoming_connection.addr)).start()

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
        # We want to close the connection whenever we exit this function no matter the cause
        with closing(incoming_connection.connection) as client_connection:
            message_handler = ClientConnectionMessageReader(
                incoming_connection.connection, self.model)
            message = message_handler.read_message()
            try:
                was_registration = self.handle_possible_registration_request(client_connection,
                                                                             message)
                if was_registration:
                    # If it was a registration message, we must wait for the second public key message
                    message = message_handler.read_message()
            except FailedToRegisterClient:
                self.logger.error("Registering client failed")
                return

            # At this point the client should be registered
            if not self.model.does_client_uuid_exist(message.header.client_id):
                self.logger.error(
                    f"{message.header.client_id} does not exist. Closing connection")
                return

            self.handle_public_key_message(client_connection, message)

            message = message_handler.read_message()
            self.handle_upload_file_message(client_connection,
                                            message)
            # The upload file message can be sent again, or a checksum ok, or final checksum incorrect
            self.handle_post_file_upload(client_connection, message_handler)

    def handle_post_file_upload(self, client_connection: AbstractConnectionInterface,
                                message_handler: ClientConnectionMessageReader):
        """
        We should call this function after a client uploaded a file.
        Checks the response from the client. If it is a FileCRCIncorrectWillRetryMessage, we 
        delete the previously uploaded file and wait for another upload.
        """
        message = message_handler.read_message()
        while isinstance(message.payload, FileCRCIncorrectWillRetryMessage):
            self.logger.error(f"{message.header.client_id} sent crc was wrong")
            self.handle_file_crc_incorrect(message)
            message = message_handler.read_message()
            self.handle_upload_file_message(
                client_connection, message)
            message = message_handler.read_message()
        # Client either accepted our checksum, or gave up
        if isinstance(message.payload, FileCRCIncorrectGivingUpMessage):
            self.logger.error(f"{message.header.client_id} Gave up")
            self.handle_file_crc_incorrect(message)
        elif isinstance(message.payload, FileCRCOKMessage):
            self.handle_file_crc_correct(client_connection, message)
        else:
            raise WrongMessageReceived(message)

    def handle_file_crc_correct(self, client_connection: AbstractConnectionInterface,
                                message: ClientMessageWithHeader):
        if not isinstance(message.payload, FileCRCOKMessage):
            raise WrongMessageReceived(message)

        self.logger.info(f"{message.header.client_id} sent crc was correct")
        self.model.set_file_verified(
            message.header.client_id, message.payload.filename)
        client_connection.send(
            SuccessResponseMessage(self.SERVER_VERSION).pack())

    def handle_file_crc_incorrect(self, message: ClientMessageWithHeader):
        """
        Accept FileCRCIncorrectWillRetryMessage or a FileCRCIncorrectGivingUpMessage message.
        either way we delete the file and remove it's entry from the database
        """
        if not (isinstance(message.payload, FileCRCIncorrectWillRetryMessage) or isinstance(message.payload, FileCRCIncorrectGivingUpMessage)):
            raise WrongMessageReceived(message)
        self.logger.info(
            f"Deleting and removing file: {message.payload.filename} for {message.header.client_id}")
        self.file_manager.remove_file(self.model.get_file_path(
            message.header.client_id, message.payload.filename))
        self.model.remove_file(message.header.client_id,
                               message.payload.filename)

    def handle_upload_file_message(self, client_connection: AbstractConnectionInterface,
                                   message: ClientMessageWithHeader):
        """
        Accept a UploadFileMessage from the client. Calculate the crc on the plain file, store it and send a
        UploadFileSuccessfulMessage response.
        """
        if not isinstance(message.payload, UploadFileMessage):
            raise WrongMessageReceived(message)

        self.logger.info(
            f"{message.header.client_id} uploaded a file: {message.payload.filename}")
        aes_key = self.model.get_client_aes_key(message.header.client_id)
        plain_content = self.encryption_utils.aes_decrypt(
            message.payload.content, aes_key)
        checksum = crc32(plain_content)
        local_path = self.file_manager.store_file(plain_content)

        self.model.store_file(message.header.client_id,
                              message.payload.filename, local_path)
        response = UploadFileSuccessfulMessage(self.SERVER_VERSION,
                                               message.header.client_id,
                                               len(message.payload.content),
                                               message.payload.filename, checksum)
        client_connection.send(response.pack())

    def handle_public_key_message(self, client_connection: AbstractConnectionInterface,
                                  message: ClientMessageWithHeader):
        if not isinstance(message.payload, ClientPublicKeyMessage):
            raise WrongMessageReceived(message)

        self.logger.info(f"Updating public key for {message.header.client_id}")
        # It is specified the AES should be uniquly generated for each session with a client
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
        self.logger.debug(f"Trying to register new client")
        if not isinstance(registration_message.payload, ClientRegistrationRequest):
            raise WrongMessageReceived(registration_message)

        try:
            client_name = registration_message.payload.name
            if self.model.is_client_registered(client_name):
                self.logger.error(
                    f"Client: {client_name} is already registered")
                raise ClientAlreadyRegisteredException(client_name)

            # We still don't know the public or aes key
            new_client_uuid = uuid.uuid4()
            client = Client(new_client_uuid,
                            registration_message.payload.name,
                            b"",
                            datetime.now(),
                            b"")
            self.model.register_client(client)
            self.logger.info(f"Registered new client with {new_client_uuid}")
            response = RegistrationSuccessfulMessage(self.SERVER_VERSION,
                                                     new_client_uuid)
            client_connection.send(response.pack())
        except Exception as e:
            self.logger.exception(f"Failed to register client: {str(e)}")
            response = RegistrationFailedMessage(self.SERVER_VERSION)
            client_connection.send(response.pack())
            raise FailedToRegisterClient()

    def close(self):
        self.connection.close()
