import unittest
import struct
from pathlib import Path
from typing import cast
from ipaddress import IPv4Address
from unittest.mock import MagicMock
from backup_client.connection_manager import AbstractConnectionManager
from backup_client.server_info import ServerInfo
from backup_client.client import Client, ErrorResponseException
from backup_client.protocol import (
    ResponseOP,
    NoBackupFilesForClientResponse, FailedToParseMessageException,
    SuccessfulBackupOrDeleteResponse, SuccessfulRestoreResponse,
)


class MockConnectionManager(AbstractConnectionManager):
    connect = MagicMock()
    send = MagicMock()
    recv = MagicMock()
    close = MagicMock()


class ClientTest(unittest.TestCase):
    def setUp(self) -> None:
        self.client.close()
        connect_fn = cast(MagicMock, self.mock_connection.connect)
        close_fn = cast(MagicMock, self.mock_connection.close)
        recv_fn = cast(MagicMock, self.mock_connection.recv)
        connect_fn.reset_mock(return_value=True, side_effect=True)
        close_fn.reset_mock(return_value=True, side_effect=True)
        recv_fn.reset_mock(return_value=True, side_effect=True)

    @classmethod
    def setUpClass(cls):
        server_info = ServerInfo(IPv4Address("1.2.3.4"), 1337)
        cls.mock_connection = MockConnectionManager()
        cls.client = Client(server_info, cls.mock_connection)

    def test_connect(self):
        self.client.connect()
        connect_fn = cast(MagicMock, self.mock_connection.connect)
        connect_fn.assert_called_once()
        self.assertTrue(self.client.is_connected)

    def test_close(self):
        self.client.close()
        close_fn = cast(MagicMock, self.mock_connection.close)
        close_fn.assert_called_once()
        self.assertFalse(self.client.is_connected)

    def test_connect_close(self):
        self.client.connect()
        self.client.close()
        self.client.connect()
        self.client.connect()
        self.client.close()

        connect_fn = cast(MagicMock, self.mock_connection.connect)
        close_fn = cast(MagicMock, self.mock_connection.close)
        self.assertEqual(3, connect_fn.call_count)
        self.assertEqual(2, close_fn.call_count)
        self.assertFalse(self.client.is_connected)

    def test_ensure_connected(self):
        self.assertFalse(self.client.is_connected)
        with self.assertRaises(FailedToParseMessageException):
            self.client.get_available_backup_files()
        connect_fn = cast(MagicMock, self.mock_connection.connect)
        connect_fn.assert_called_once()
        self.assertFalse(self.client.is_connected)

    def test_get_backup_list(self):
        filename = "coolname"
        payload = "file1\nfile2\nfile3\n"
        side_effects = [
            struct.pack("<BH", 123, ResponseOP.SUCCESSFUL_LIST_FILES.value),
            struct.pack("<H", len("coolname")),
            struct.pack(f"<{len(filename)}s", filename.encode()),
            struct.pack("<I", len(payload)),
            struct.pack(f"<{len(payload)}s", payload.encode())
        ]
        cast(MagicMock, self.mock_connection.recv).side_effect = side_effects
        files = self.client.get_available_backup_files()
        self.assertEqual(["file1", "file2", "file3"], files)

    def test_get_backup_list_no_files_response(self):
        response = NoBackupFilesForClientResponse(222)
        side_effects = [struct.pack(
            "<BH", 222, ResponseOP.NO_BACKUP_FILES_FOR_CLIENT.value)]
        cast(MagicMock, self.mock_connection.recv).side_effect = side_effects
        with self.assertRaises(ErrorResponseException) as e:
            self.client.get_available_backup_files()

        self.assertEqual(e.exception.args[0], str(response))

    def test_backup_file(self):
        filename = "coolfile.txt"
        with open(filename, "w") as f:
            f.write("thisisthepayload\n")

        response = SuccessfulBackupOrDeleteResponse(112, filename)
        side_effects = [
            struct.pack(
                "<BH", 112, ResponseOP.SUCCESSFUL_BACKUP_OR_DELETE.value),
            struct.pack("<H", len(filename)),
            struct.pack(f"<{len(filename)}s", filename.encode())
        ]
        cast(MagicMock, self.mock_connection.recv).side_effect = side_effects
        filename = self.client.backup_file(Path(filename))
        self.assertEqual(response.filename, filename)

    def test_restore_file(self):
        filename = "torestore.jpeg"
        payload = "thiswasthefile\n"
        response = SuccessfulRestoreResponse(234, filename, payload.encode())

        side_effects = [
            struct.pack("<BH", 234, ResponseOP.SUCCESSFUL_RESTORE.value),
            struct.pack("<H", len(filename)),
            struct.pack(f"<{len(filename)}s", filename.encode()),
            struct.pack("<I", len(payload)),
            struct.pack(f"<{len(payload)}s", payload.encode())
        ]
        cast(MagicMock, self.mock_connection.recv).side_effect = side_effects
        restored_payload = self.client.restore_file(filename)
        self.assertEqual(response.payload, restored_payload)

    def delete_file(self):
        filename = "deletethis.out"
        response = SuccessfulBackupOrDeleteResponse(111, filename)
        side_effects = [
            struct.pack(
                "<BH", 111, ResponseOP.SUCCESSFUL_BACKUP_OR_DELETE.value),
            struct.pack("<H", len(filename)),
            struct.pack(f"<{len(filename)}s", filename.encode()),
        ]
        cast(MagicMock, self.mock_connection.recv).side_effect = side_effects
        restored_payload = self.client.delete_file(filename)
        self.assertEqual(response.filename, restored_payload)


if __name__ == "__main__":
    unittest.main()
