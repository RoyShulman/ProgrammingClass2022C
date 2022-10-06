import logging
import threading
from ipaddress import IPv4Address
from server.connection_interface import AbstractConnectionInterface, Address, IncomingConnection
from server.protocol.client_message import ClientMessageParser, ClientMessageReader, ClientMessageWithHeader, ClientMessageCode
from server.protocol.server_message import RegistrationSuccessfulMessage


class Server:
    NUM_LISTENING_CONNECTIONS = 10
    SERVER_VERSION = 3

    def __init__(self, port: int, connection: AbstractConnectionInterface) -> None:
        self.port = port
        self.connection = connection
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
        """
        self.logger.info(f"New connection from {incoming_connection.addr}")
        reader = ClientMessageReader(incoming_connection.connection)
        message = ClientMessageParser.parse_message(reader)
        self.handle_initial_client_message(
            incoming_connection.connection, message)

    def handle_initial_client_message(self, client_connection: AbstractConnectionInterface, initial_message: ClientMessageWithHeader):
        if initial_message.header.code == ClientMessageCode.REGISTRATION:
            self.register_client(client_connection, initial_message)
        # elif initial_message.header.code == ClientMessageCode.CLIENT_PUBLIC_KEY:

    def register_client(self, client_connection: AbstractConnectionInterface, registration_message: ClientMessageWithHeader):
        self.logger.info(f"Registering new client")
        response = RegistrationSuccessfulMessage(self.SERVER_VERSION, registration_message.header.client_id)
        client_connection.send(response.pack())

    # def update_client_encryption_keys(self, client_connection: AbstractConnectionInterface, public_key_message: ClientMessageWithHeader):

    def close(self):
        self.connection.close()
