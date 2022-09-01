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
        self.socket = socket.socket()

    def connect(self, dst: Tuple[IPv4Address, int]) -> None:
        self.socket.connect(dst)

    def send(self, data: bytes) -> None:
        self.socket.send(data)

    def recv(self, len: int) -> bytes:
        return self.socket.recv(len)

    def close(self) -> None:
        self.socket.close()
