import unittest
import struct
import secrets
from unittest.mock import MagicMock
from typing import cast
from uuid import uuid4
from server.protocol.client_message import (
    AbstractClientMessageReader,
    ClientMessageHeader,
    ClientMessageCode,
    ClientRegistrationRequest,
    ClientPublicKeyMessage,
    UploadFileMessage
)


class MockClientMessageReader(AbstractClientMessageReader):
    read = MagicMock()


class ClientMessageTest(unittest.TestCase):
    def setUp(self) -> None:
        self.client_uuid = uuid4()
        self.reader = MockClientMessageReader()
        self.mock_reader = cast(MagicMock, self.reader.read)

    def tearDown(self) -> None:
        cast(MagicMock, self.reader.read).reset_mock()

    def test_unpack_header(self):
        self.mock_reader.side_effect = [struct.pack("<16sBHI", self.client_uuid.bytes,
                                                    3, ClientMessageCode.REGISTRATION.value, 8)]

        message = ClientMessageHeader.unpack(self.reader)

        header = ClientMessageHeader(self.client_uuid, 3,
                                     ClientMessageCode.REGISTRATION, 8)
        self.assertEqual(header, message)
        self.mock_reader.assert_called_once_with(struct.calcsize("<16sBHI"))

    def test_unpack_client_registration(self):
        name = struct.pack("<255s", b"coolname")
        self.mock_reader.side_effect = [name]
        message = ClientRegistrationRequest.unpack(self.reader)

        expected_message = ClientRegistrationRequest(name.decode())
        self.assertEqual(expected_message, message)
        self.mock_reader.assert_called_once_with(struct.calcsize("<255s"))

    def test_unpack_public_key(self):
        public_key = secrets.token_bytes(160)
        name = struct.pack("<255s", b"coolname")

        self.mock_reader.side_effect = [
            struct.pack("<255s160s", name, public_key)]
        message = ClientPublicKeyMessage.unpack(self.reader)

        expected_message = ClientPublicKeyMessage(name.decode(), public_key)
        self.assertEqual(expected_message, message)
        self.mock_reader.assert_called_once_with(struct.calcsize("<255s160s"))

    def test_unpack_upload_file(self):
        content_size = 5
        filename = struct.pack("<255s", b"filename.jpeg")
        content = b"12345"

        self.mock_reader.side_effect = [
            struct.pack("<16sI255s", self.client_uuid.bytes,
                        content_size, filename),
            struct.pack("<5s", content)]
        message = UploadFileMessage.unpack(self.reader)

        expected_message = UploadFileMessage(self.client_uuid,
                                             filename.decode(), content)
        self.assertEqual(expected_message, message)
        self.assertEqual(2, self.mock_reader.call_count)
        self.mock_reader.assert_called_with(struct.calcsize("<5s"))


if __name__ == "__main__":
    unittest.main()
