import unittest
import struct
from unittest.mock import MagicMock
from typing import cast
from uuid import uuid4
from server.protocol.client_message import (
    AbstractClientMessageReader,
    ClientMessageHeader,
    ClientMessageCode
)


class MockClientMessageReader(AbstractClientMessageReader):
    read = MagicMock()


class ClientMessageTest(unittest.TestCase):
    def setUp(self) -> None:
        self.reader = MockClientMessageReader()

    def tearDown(self) -> None:
        cast(MagicMock, self.reader.read).reset_mock()

    def test_unpack_header(self):
        client_uuid = uuid4()
        mock_reader = cast(MagicMock, self.reader.read)
        mock_reader.return_value = struct.pack("<16sBHI", client_uuid.bytes,
                                               3, ClientMessageCode.REGISTRATION.value, 8)

        message = ClientMessageHeader.unpack(self.reader)

        header = ClientMessageHeader(client_uuid, 3,
                                     ClientMessageCode.REGISTRATION, 8)
        self.assertEqual(header, message)
        mock_reader.assert_called_once_with(struct.calcsize("<16sBHI"))


if __name__ == "__main__":
    unittest.main()
