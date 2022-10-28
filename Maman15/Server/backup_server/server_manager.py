from pathlib import Path
from contextlib import closing, contextmanager
from typing import Iterator
from backup_server.connection_interface import AbstractConnectionInterface
from backup_server.abstract_encryption_utils import AbstractEncryptionUtils
from backup_server.server import Server
from backup_server.port_info import PortInfo, PortInfoFileNotFound
from backup_server.server_model import AbstractServerModel


class ServerManager:
    """
    Class to create a server with the wanted port. 
    It first tries to read the port from a file, and if the file isn't there it 
    defaults to 1234.
    This class also closes the server connection when needed, or on any error
    """
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
