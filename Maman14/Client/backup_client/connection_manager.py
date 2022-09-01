import socket
from abc import ABC, abstractmethod
from ipaddress import ip_address


class AbstractConnectionManager(ABC):
    def __init__(self, ip: str, port: int) -> None:
        self.ip = ip_address(ip)
        self.port = port

    @abstractmethod
    def connect(self) -> None:
        raise NotImplementedError

    @abstractmethod
    def send(self, data: bytes) -> None:
        raise NotImplementedError

    @abstractmethod
    def recv(self, len: int) -> bytes:
        raise NotImplementedError

    @abstractmethod
    def close(self) -> None:
        raise NotImplementedError


class NetworkConnectionManager(AbstractConnectionManager):
    def __init__(self, ip: str, port: int) -> None:
        super().__init__(ip, port)
        self.socket = socket.socket()

    def connect(self) -> None:
        self.socket.connect((self.ip, self.port))

    def send(self, data: bytes) -> None:
        self.socket.send(data)

    def recv(self, len: int) -> bytes:
        return self.socket.recv(len)

    def close(self) -> None:
        self.socket.close()
