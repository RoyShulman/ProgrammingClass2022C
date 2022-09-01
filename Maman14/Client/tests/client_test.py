import unittest
from typing import cast
from ipaddress import IPv4Address
from unittest.mock import MagicMock
from backup_client.connection_manager import AbstractConnectionManager
from backup_client.server_info import ServerInfo
from backup_client.backup_info import BackupInfo
from backup_client.client import Client


class MockConnectionManager(AbstractConnectionManager):
    connect = MagicMock()
    send = MagicMock()
    recv = MagicMock()
    close = MagicMock()


class ClientTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        server_info = ServerInfo(IPv4Address("1.2.3.4"), 1337)
        backup_info = BackupInfo([])
        cls.mock_connection = MockConnectionManager()
        cls.client = Client(server_info, backup_info, cls.mock_connection)

    def test_connect(self):
        self.client.connect()
        self.client.connect()
        connect_fn = cast(MagicMock, self.mock_connection.connect)
        connect_fn.assert_called_once()
        self.assertTrue(self.client.is_connected)

    def test_close(self):
        self.client.close()
        close_fn = cast(MagicMock, self.mock_connection.close)
        close_fn.assert_called_once()

    def test_ensure_connected(self):
        self.client.get_available_backup_files()
        connect_fn = cast(MagicMock, self.mock_connection.connect)
        connect_fn.assert_called_once()
        self.assertTrue(self.client.is_connected)


if __name__ == "__main__":
    unittest.main()
