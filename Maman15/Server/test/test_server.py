import unittest
import struct
import uuid
import logging
import sys
from ipaddress import IPv4Address
from unittest.mock import MagicMock
from typing import cast
from ipaddress import IPv4Address
from server.server import Server
from server.connection_interface import AbstractConnectionInterface, IncomingConnection, Address
from server.protocol.client_message import ClientMessageCode, ClientMessageHeader
from server.protocol.server_message import RegistrationSuccessfulMessage


class MockConnectionInterface(AbstractConnectionInterface):
    recv = MagicMock()
    close = MagicMock()
    send = MagicMock()
    bind = MagicMock()
    listen = MagicMock()
    accept = MagicMock()


def pack_header(header: ClientMessageHeader) -> bytes:
    return struct.pack("<16sBHI", header.client_id.bytes, header.client_version, header.code.value, header.payload_size)


class ServerTest(unittest.TestCase):
    def setUp(self) -> None:
        logging.basicConfig(stream=sys.stdout)
        self.client_connection = MockConnectionInterface()
        self.recv = cast(MagicMock, self.client_connection.recv)
        self.send = cast(MagicMock, self.client_connection.send)
        self.server = Server(1337, MockConnectionInterface())
        self.incoming_connection = IncomingConnection(self.client_connection,
                                                      Address(IPv4Address("1.2.3.4"), 1234))

    def tearDown(self) -> None:
        self.recv.reset_mock()
        self.send.reset_mock()

    def test_handle_client_connection(self):
        client_uuid = uuid.uuid4()
        self.recv.side_effect = [pack_header(ClientMessageHeader(client_uuid, 3, ClientMessageCode.REGISTRATION, 255)),
                                 struct.pack("<255s", b"bestname")]
        self.server.handle_client_connection(self.incoming_connection)
        self.send.assert_called_once_with(RegistrationSuccessfulMessage(self.server.SERVER_VERSION,
                                                                        client_uuid).pack())


if __name__ == "__main__":
    unittest.main()
