import logging
import threading
from datetime import datetime
from ipaddress import IPv4Address
from server.connection_interface import AbstractConnectionInterface, Address, IncomingConnection
from server.protocol.client_message import (
    ClientMessageParser,
    ClientMessageReader,
    ClientMessageWithHeader,
    ClientMessageCode,
    ClientRegistrationRequest,
    ClientPublicKeyMessage)
from server.protocol.server_message import (
    RegistrationSuccessfulMessage,
    AESKeyMessage
)
from server.server_model import AbstractServerModel, Client
from server.encryption_utils import AbstractEncryptionUtils


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

    def __init__(self, port: int, connection: AbstractConnectionInterface, model: AbstractServerModel,
                 encryption_utils: AbstractEncryptionUtils) -> None:
        self.port = port
        self.connection = connection
        self.model = model
        self.encryption_utils = encryption_utils
        self.logger = logging.getLogger(__name__)

    def server_requests(self) -> None:
        self.logger.info(f"Starting to serve requests on port: {self.port}")
        self.connection.bind(Address(IPv4Address(""), self.port))
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

    def handle_public_key_message(self, client_connection: AbstractConnectionInterface,
                                  message: ClientMessageWithHeader):
        if not isinstance(message.payload, ClientPublicKeyMessage):
            raise WrongMessageReceived(message)

        self.model.update_client_public_key(
            message.header.client_id, message.payload.public_key)
        aes_key = self.encryption_utils.get_aes_key(self.AES_KEY_SIZE)
        response = AESKeyMessage(self.SERVER_VERSION,
                                 message.header.client_id,
                                 aes_key)
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
