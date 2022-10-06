import unittest
import uuid
from datetime import datetime
from pathlib import Path
from backup_server.server_model import ServerModel, Client, ClientWithSpecifiedUUIDNotFound


class ServerModelTest(unittest.TestCase):
    def setUp(self) -> None:
        self.db_path = Path("server.db")
        self.model = ServerModel(self.db_path)

    def tearDown(self) -> None:
        self.db_path.unlink()

    def test_register_client(self):
        client = Client(uuid.uuid4(), "coolname", b"", datetime.now(), b"")
        self.model.register_client(client)

        registered_clients = self.model.get_clients()
        self.assertEqual(1, len(registered_clients))
        self.assertEqual(registered_clients[0], client)

    def test_get_client(self):
        client = Client(uuid.uuid4(), "coolname", b"", datetime.now(), b"")
        self.model.register_client(client)

        self.assertEqual(client, self.model.get_client(client.uuid))

    # def test_client_not_found(self):
    #     with self.assertRaises(ClientWithSpecifiedUUIDNotFound):
    #         self.model.get_client(uuid.uuid4())

    # def test_update_client(self):
    #     client = Client(uuid.uuid4(), "coolname", b"", datetime.now(), b"")
    #     self.model.register_client(client)

    #     client.aes_key = b"newkey"
    #     self.assertNotEqual(self.model.get_clients()[0], client)

    #     self.model.update_client(client)
    #     self.assertEqual(self.model.get_clients()[0], client)


if __name__ == "__main__":
    unittest.main()
