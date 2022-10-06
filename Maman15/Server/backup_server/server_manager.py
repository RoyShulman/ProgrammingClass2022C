from pathlib import Path
from contextlib import closing, contextmanager
from typing import Iterator
from backup_server.connection_interface import AbstractConnectionInterface
from backup_server.encryption_utils import AbstractEncryptionUtils
from backup_server.server import Server
from backup_server.port_info import PortInfo, PortInfoFileNotFound
from backup_server.server_model import AbstractServerModel


class ServerManager:
    DEFAULT_PORT = 1234

    def __init__(self, connection: AbstractConnectionInterface, model: AbstractServerModel,
                 encryption_utils: AbstractEncryptionUtils, base_storage_path: Path) -> None:
        self.connection = connection
        self.model = model
        self.encryption_utils = encryption_utils
        self.base_storage_path = base_storage_path

    @contextmanager
    def get_server(self) -> Iterator[Server]:
        try:
            port = PortInfo.from_file().port
        except PortInfoFileNotFound:
            port = self.DEFAULT_PORT
        with closing(Server(port, self.connection, self.model,
                            self.encryption_utils, self.base_storage_path)) as server:
            yield server
