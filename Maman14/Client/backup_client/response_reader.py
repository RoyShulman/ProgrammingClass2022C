from backup_client.connection_manager import AbstractConnectionManager


class ResponseReader:
    """
    This class abstract the connectio manager for reading responses
    """

    def __init__(self, connection_manager: AbstractConnectionManager) -> None:
        self.connection_manager = connection_manager

    def read(self, size: int) -> bytes:
        return self.connection_manager.recv(size)
