import unittest
import struct
import uuid
import logging
import sys
from ipaddress import IPv4Address
from unittest.mock import MagicMock, ANY
from typing import cast
from ipaddress import IPv4Address
from server.server import Server, ClientAlreadyRegisteredException, WrongMessageReceived
from server.connection_interface import AbstractConnectionInterface, IncomingConnection, Address
from server.protocol.client_message import (
    ClientMessageCode,
    ClientMessageHeader,
    ClientMessageWithHeader,
    ClientRegistrationRequest,
    ClientPublicKeyMessage)
from server.protocol.server_message import AESKeyMessage, RegistrationSuccessfulMessage
from server.server_model import AbstractServerModel, Client
from server.encryption_utils import AbstractEncryptionUtils


class MockConnectionInterface(AbstractConnectionInterface):
    recv = MagicMock()
    close = MagicMock()
    send = MagicMock()
    bind = MagicMock()
    listen = MagicMock()
    accept = MagicMock()


class MockServerModel(AbstractServerModel):
    create_tables_if_needed = MagicMock()
    get_clients = MagicMock()
    get_client = MagicMock()
    is_client_registered = MagicMock()
    register_client = MagicMock()
    update_client_public_key = MagicMock()


class MockEncryptionUtils(AbstractEncryptionUtils):
    get_aes_key = MagicMock(return_value=b"1234567890123456")


def pack_header(header: ClientMessageHeader) -> bytes:
    return struct.pack("<16sBHI", header.client_id.bytes, header.client_version, header.code.value, header.payload_size)


class ServerTest(unittest.TestCase):
    def setUp(self) -> None:
        logging.basicConfig(stream=sys.stdout)
        self.client_connection = MockConnectionInterface()
        self.model = MockServerModel()
        self.encryption = MockEncryptionUtils()
        self.recv = cast(MagicMock, self.client_connection.recv)
        self.send = cast(MagicMock, self.client_connection.send)
        self.server = Server(1337, MockConnectionInterface(),
                             self.model, self.encryption)
        self.incoming_connection = IncomingConnection(self.client_connection,
                                                      Address(IPv4Address("1.2.3.4"), 1234))

    def tearDown(self) -> None:
        self.recv.reset_mock()
        self.send.reset_mock()

    def test_register_client(self):
        client_uuid = uuid.uuid4()
        header = ClientMessageHeader(client_uuid, 3,
                                     ClientMessageCode.REGISTRATION, 255)
        payload = ClientRegistrationRequest("bestname")
        message = ClientMessageWithHeader(header, payload)

        cast(MagicMock, self.model.is_client_registered).side_effect = [False]
        self.server.register_client(self.incoming_connection.connection,
                                    message)

        self.send.assert_called_once_with(RegistrationSuccessfulMessage(self.server.SERVER_VERSION,
                                                                        client_uuid).pack())
        cast(MagicMock, self.model.register_client).assert_called_once_with(Client(client_uuid,
                                                                                   "bestname", b"", ANY, b""))

    def test_registered_already_rgistered(self):
        client_uuid = uuid.uuid4()
        header = ClientMessageHeader(client_uuid, 3,
                                     ClientMessageCode.REGISTRATION, 255)
        payload = ClientRegistrationRequest("bestname")
        message = ClientMessageWithHeader(header, payload)

        cast(MagicMock, self.model.is_client_registered).side_effect = [True]
        with self.assertRaises(ClientAlreadyRegisteredException):
            self.server.register_client(self.incoming_connection.connection,
                                        message)

    @staticmethod
    def get_public_key_message() -> ClientMessageWithHeader:
        client_uuid = uuid.uuid4()
        header = ClientMessageHeader(client_uuid, 3,
                                     ClientMessageCode.CLIENT_PUBLIC_KEY, 255)
        payload = ClientPublicKeyMessage("bestname", b"1234")
        return ClientMessageWithHeader(header, payload)

    def test_wrong_message_register_client(self):
        message = self.get_public_key_message()
        with self.assertRaises(WrongMessageReceived):
            self.server.register_client(self.incoming_connection.connection,
                                        message)

    def test_handle_client_public_key(self):
        message = self.get_public_key_message()
        assert isinstance(message.payload,
                          ClientPublicKeyMessage)  # for pylance
        self.server.handle_public_key_message(self.incoming_connection.connection,
                                              message)

        self.send.assert_called_once_with(AESKeyMessage(self.server.SERVER_VERSION,
                                                        message.header.client_id,
                                                        b"1234567890123456").pack())
        cast(MagicMock, self.model.update_client_public_key).assert_called_once_with(message.header.client_id,
                                                                                     message.payload.public_key)

        # def test_handle_client_connection(self):
        #     client_uuid = uuid.uuid4()
        #     self.recv.side_effect = [pack_header(ClientMessageHeader(client_uuid, 3, ClientMessageCode.REGISTRATION, 255)),
        #                              struct.pack("<255s", b"bestname")]
        #     self.server.handle_client_connection(self.incoming_connection)
        #     self.send.assert_called_once_with(RegistrationSuccessfulMessage(self.server.SERVER_VERSION,
        #                                                                     client_uuid).pack())


if __name__ == "__main__":
    unittest.main()
