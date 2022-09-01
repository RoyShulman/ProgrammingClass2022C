import unittest
import os
from ipaddress import IPv4Address
from backup_client.server_info import ServerInfo, InvalidServerInfo, ReadingServerInfoFileException


class ServerInfoTest(unittest.TestCase):
    def setUp(self) -> None:
        try:
            os.unlink("server.info")
        except FileNotFoundError:
            pass

    def tearDown(self) -> None:
        try:
            os.unlink("server.info")
        except FileNotFoundError:
            pass

    def test_from_addr(self):
        server_info = ServerInfo.from_address("127.0.0.1:1337")
        self.assertEqual(IPv4Address("127.0.0.1"), server_info.ip)
        self.assertEqual(1337, server_info.port)

    def test_from_file(self):
        with open("server.info", "w") as f:
            f.write("127.0.0.1:1337")
        info = ServerInfo.from_file()
        self.assertEqual(IPv4Address("127.0.0.1"), info.ip)
        self.assertEqual(1337, info.port)

    def test_nonexistent_file(self):
        self.assertRaises(ReadingServerInfoFileException, ServerInfo.from_file)

    def test_empty_file(self):
        open("server.info", "w").close()
        self.assertRaises(InvalidServerInfo, ServerInfo.from_file)

    def test_bad_ip(self):
        self.assertRaises(InvalidServerInfo,
                          ServerInfo.from_address, "303.2.3.4:123")

    def test_bad_port(self):
        self.assertRaises(InvalidServerInfo,
                          ServerInfo.from_address, "1.2.3.4:80a")

    def test_to_raw(self):
        server_info = ServerInfo.from_address("127.0.0.1:1337")
        self.assertEqual(("127.0.0.1", 1337), server_info.to_raw())


if __name__ == "__main__":
    unittest.main()
