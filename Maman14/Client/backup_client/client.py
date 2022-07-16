import random
from pathlib import Path
from contextlib import contextmanager
from typing import Callable, Any, List, Type, cast
from functools import wraps
from backup_client.server_info import ServerInfo
from backup_client.connection_manager import AbstractConnectionManager
from backup_client.protocol import (
    ResponseParser, ProtocolRequest, ProtocolResponse,
    ListFilesRequest, ListFilesResponse,
    BackupFileRequest, SuccessfulBackupOrDeleteResponse,
    RestoreFileRequest, SuccessfulRestoreResponse,
    DeleteFileRequest,
)
from backup_client.response_reader import ResponseReader


def ensure_connected(func: Callable[..., Any]):
    @wraps(func)
    def with_connection(self: 'Client', *args: Any, **kwargs: Any):
        try:
            if not self.is_connected:
                self.connect()
            return func(self, *args, **kwargs)
        finally:
            self.close()
    return with_connection


class UnexpectedResponseException(Exception):
    pass


class ErrorResponseException(Exception):
    pass


class Client:
    MAX_USER_ID = 2**32 - 1
    MIN_USER_ID = 1

    VERSION = 1

    def __init__(self, server_info: ServerInfo, connection_manager: AbstractConnectionManager) -> None:
        self.user_id = random.randint(self.MIN_USER_ID, self.MAX_USER_ID)
        self.server_info = server_info
        self.connection_manager = connection_manager
        self.response_reader = ResponseReader(self.connection_manager)
        self.is_connected = False

    def connect(self) -> None:
        self.connection_manager.connect(self.server_info.to_tuple())
        self.is_connected = True

    @classmethod
    def from_file(cls, connection_manager: AbstractConnectionManager) -> 'Client':
        return cls(ServerInfo.from_file(Path(__file__).parent), connection_manager)

    def close(self) -> None:
        self.connection_manager.close()
        self.is_connected = False

    @staticmethod
    def raise_if_error(response: ProtocolResponse):
        if response.is_error():
            raise ErrorResponseException(str(response))

    @staticmethod
    def raise_if_unexpected_response(response: ProtocolResponse, expected_type: Type[ProtocolResponse]):
        Client.raise_if_error(response)
        if not isinstance(response, expected_type):
            raise UnexpectedResponseException(response)

    def send_recv_message(self, request: ProtocolRequest, expected_response_type: Type[ProtocolResponse]) -> ProtocolResponse:
        self.connection_manager.send(request.pack())
        response = ResponseParser.parse_message(self.response_reader)
        self.raise_if_unexpected_response(response, expected_response_type)
        return response

    @ensure_connected
    def get_available_backup_files(self) -> List[str]:
        print(f"Getting available backup files")
        request = ListFilesRequest(self.user_id, self.VERSION)
        response = self.send_recv_message(request, ListFilesResponse)

        # Just for type hints to work correctly
        response = cast(ListFilesResponse, response)
        return [filename.strip() for filename in response.payload.decode().splitlines()]

    @ensure_connected
    def backup_file(self, filepath: Path) -> str:
        print(f"Backup file {filepath}")
        request = BackupFileRequest(
            self.user_id, self.VERSION, filepath.name, filepath.read_bytes())
        response = self.send_recv_message(
            request, SuccessfulBackupOrDeleteResponse)

        response = cast(SuccessfulBackupOrDeleteResponse, response)
        return response.filename

    @ensure_connected
    def restore_file(self, filename: str) -> bytes:
        print(f"Restoring file {filename}")
        request = RestoreFileRequest(self.user_id,
                                     self.VERSION,
                                     filename)
        response = self.send_recv_message(request, SuccessfulRestoreResponse)

        response = cast(SuccessfulRestoreResponse, response)
        print(f"Successfully restored {response.filename}")
        return response.payload

    @ensure_connected
    def delete_file(self, filename: str):
        print(f"Deleting {filename}")
        request = DeleteFileRequest(self.user_id,
                                    self.VERSION, filename)
        response = self.send_recv_message(request,
                                          SuccessfulBackupOrDeleteResponse)
        response = cast(SuccessfulBackupOrDeleteResponse, response)
        print(response)


@contextmanager
def get_client_from_file(connection_manager: AbstractConnectionManager):
    client = Client.from_file(connection_manager)
    try:
        yield client
    finally:
        client.close()
