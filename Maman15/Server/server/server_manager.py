from pathlib import Path
from server.connection_interface import AbstractConnectionInterface
from server.encryption_utils import AbstractEncryptionUtils
from server.server import Server
from server.port_info import PortInfo, PortInfoFileNotFound
from server.server_model import AbstractServerModel


class ServerManager:
    DEFAULT_PORT = 1234

    def __init__(self, connection: AbstractConnectionInterface, model: AbstractServerModel,
                 encryption_utils: AbstractEncryptionUtils, base_storage_path: Path) -> None:
        self.connection = connection
        self.model = model
        self.encryption_utils = encryption_utils
        self.base_storage_path = base_storage_path

    def get_server(self) -> Server:
        try:
            port = PortInfo.from_file().port
        except PortInfoFileNotFound:
            port = self.DEFAULT_PORT
        return Server(port, self.connection, self.model,
                      self.encryption_utils, self.base_storage_path)
