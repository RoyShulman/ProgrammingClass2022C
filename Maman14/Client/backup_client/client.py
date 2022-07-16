import random
from backup_client.backup_info import BackupInfo
from backup_client.server_info import ServerInfo


class Client:
    MAX_USER_ID = 2**32 - 1
    MIN_USER_ID = 1

    def __init__(self, server_info: ServerInfo, backup_info: BackupInfo) -> None:
        self.server_info = server_info
        self.backup_info = backup_info
        self.user_id = random.randint(self.MIN_USER_ID, self.MAX_USER_ID)

    @classmethod
    def from_file(cls):
        return cls(ServerInfo.from_file(), BackupInfo.from_file())
