import socket
from abc import ABC, abstractmethod
from ipaddress import IPv4Address
from typing import Tuple


class AbstractConnectionManager(ABC):
    @abstractmethod
    def connect(self, dst: Tuple[IPv4Address, int]) -> None:
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
    def __init__(self) -> None:
        self.socket = None

    def connect(self, dst: Tuple[IPv4Address, int]) -> None:
        print(f"Connecting to {dst}")
        self.socket = socket.socket()
        self.socket.connect((str(dst[0]), dst[1]))

    def send(self, data: bytes) -> None:
        self.socket.send(data)

    def recv(self, len: int) -> bytes:
        return self.socket.recv(len)

    def close(self) -> None:
        print(f"Closing connection")
        self.socket.close()
