from pathlib import Path
import unittest
import struct
import uuid
import logging
import sys
from ipaddress import IPv4Address
from unittest.mock import MagicMock, ANY
from typing import cast
from ipaddress import IPv4Address
from backup_server.server import FailedToRegisterClient, Server, ClientAlreadyRegisteredException, WrongMessageReceived
from backup_server.connection_interface import AbstractConnectionInterface, IncomingConnection, Address
from backup_server.protocol.client_message import (
    ClientMessageCode,
    ClientMessageHeader,
    ClientMessageWithHeader,
    ClientRegistrationRequest,
    ClientPublicKeyMessage,
    UploadFileMessage)
from backup_server.protocol.server_message import AESKeyMessage, RegistrationSuccessfulMessage, UploadFileSuccessfulMessage
from backup_server.server_model import AbstractServerModel, Client
from backup_server.encryption_utils import AbstractEncryptionUtils
from backup_server.crc32 import crc32


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
    update_client_keys = MagicMock()
    store_file = MagicMock()
    get_client_aes_key = MagicMock()
    remove_file = MagicMock()
    set_file_verified = MagicMock()
    get_file_path = MagicMock()
    update_last_seen_time = MagicMock()
    does_client_uuid_exist = MagicMock()


class MockEncryptionUtils(AbstractEncryptionUtils):
    get_aes_key = MagicMock(return_value=b"1234567890123456")
    rsa_encrypt = MagicMock(return_value=b"1"*128)
    aes_decrypt = MagicMock(return_value=b"thecontent\nmorecontent")


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
                             self.model, self.encryption, Path("/tmp"))
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

        cast(MagicMock, self.model.register_client).assert_called_once_with(Client(ANY,
                                                                                   "bestname", b"", ANY, b""))

    def test_registered_already_rgistered(self):
        client_uuid = uuid.uuid4()
        header = ClientMessageHeader(client_uuid, 3,
                                     ClientMessageCode.REGISTRATION, 255)
        payload = ClientRegistrationRequest("bestname")
        message = ClientMessageWithHeader(header, payload)

        cast(MagicMock, self.model.is_client_registered).side_effect = [True]
        with self.assertRaises(FailedToRegisterClient):
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
                                                        b"1"*128).pack())
        cast(MagicMock, self.model.update_client_keys).assert_called_once_with(message.header.client_id,
                                                                               message.payload.public_key,
                                                                               ANY)

    @staticmethod
    def get_upload_file_message() -> ClientMessageWithHeader:
        client_uuid = uuid.uuid4()
        header = ClientMessageHeader(client_uuid, 3,
                                     ClientMessageCode.UPLOAD_FILE, 255)
        payload = UploadFileMessage(
            client_uuid, "bestfilename", b"thecontent\nmorecontent")
        return ClientMessageWithHeader(header, payload)

    def test_handle_upload_file(self):
        message = self.get_upload_file_message()
        assert isinstance(message.payload,
                          UploadFileMessage)  # for pylance

        self.server.handle_upload_file_message(
            self.incoming_connection.connection, message)

        self.send.assert_called_once_with(UploadFileSuccessfulMessage(self.server.SERVER_VERSION,
                                                                      message.header.client_id,
                                                                      len(b"thecontent\nmorecontent"),
                                                                      "bestfilename",
                                                                      crc32(b"thecontent\nmorecontent")).pack())
        cast(MagicMock, self.model.store_file).assert_called_once_with(message.header.client_id,
                                                                       "bestfilename",
                                                                       ANY)


if __name__ == "__main__":
    unittest.main()
