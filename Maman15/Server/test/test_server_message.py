import unittest
import uuid
import secrets
from backup_server.protocol.server_message import (
    RegistrationSuccessfulMessage,
    AESKeyMessage,
    UploadFileSuccessfulMessage,
    SuccessResponseMessage
)


class ServerMessageTest(unittest.TestCase):
    def test_registration_message(self):
        message = RegistrationSuccessfulMessage(3, uuid.uuid4())
        self.assertEqual(3, message.server_version)
        self.assertEqual(16, message.get_payload_size())
        self.assertEqual(2100, message.code.value)

    def test_aes_key_message(self):
        message = AESKeyMessage(3, uuid.uuid4(), secrets.token_bytes(16))
        self.assertEqual(3, message.server_version)
        self.assertEqual(32, message.get_payload_size())
        self.assertEqual(2102, message.code.value)

    def test_upload_file_successful_message(self):
        message = UploadFileSuccessfulMessage(3, uuid.uuid4(),
                                              50, "coolfile.jpeg", 1234)
        self.assertEqual(3, message.server_version)
        self.assertEqual(16 + 4 + 255 + 4, message.get_payload_size())
        self.assertEqual(2103, message.code.value)

    def test_success_response_message(self):
        message = SuccessResponseMessage(3)
        self.assertEqual(3, message.server_version)
        self.assertEqual(0, message.get_payload_size())
        self.assertEqual(2104, message.code.value)


if __name__ == "__main__":
    unittest.main()
