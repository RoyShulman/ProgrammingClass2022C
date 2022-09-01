import random
from contextlib import contextmanager
from typing import Callable, Any
from functools import wraps
from backup_client.backup_info import BackupInfo
from backup_client.server_info import ServerInfo
from backup_client.connection_manager import AbstractConnectionManager
from backup_client.protocol import ListFilesRequest


@wraps
def ensure_connected(func: Callable[..., Any]):
    def with_connection(self: Client, *args: Any, **kwargs: Any):
        if not self.is_connected:
            self.connect()
        return func(args, kwargs)
    return with_connection


class Client:
    MAX_USER_ID = 2**32 - 1
    MIN_USER_ID = 1

    VERSION = 1

    def __init__(self, server_info: ServerInfo, backup_info: BackupInfo, connection_manager: AbstractConnectionManager) -> None:
        self.user_id = random.randint(self.MIN_USER_ID, self.MAX_USER_ID)
        self.server_info = server_info
        self.backup_info = backup_info
        self.connection_manager = connection_manager
        self.is_connected = False

    def connect(self) -> None:
        if self.is_connected:
            return
        self.connection_manager.connect(self.server_info.to_tuple())
        self.is_connected = True

    @classmethod
    def from_file(cls, connection_manager: AbstractConnectionManager) -> 'Client':
        return cls(ServerInfo.from_file(), BackupInfo.from_file(), connection_manager)

    def close(self) -> None:
        self.connection_manager.close()

    @ensure_connected
    def get_available_backup_files(self):
        request = ListFilesRequest(self.user_id, self.VERSION).pack()
        self.connection_manager.send(request)


@contextmanager
def get_client_from_file(connection_manager: AbstractConnectionManager):
    client = Client.from_file(connection_manager)
    try:
        client.connect()
        yield client
    finally:
        client.close()
