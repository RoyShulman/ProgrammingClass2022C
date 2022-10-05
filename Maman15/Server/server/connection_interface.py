from abc import ABC, abstractmethod
from socket import socket
from dataclasses import dataclass
from ipaddress import IPv4Address


@dataclass(frozen=True)
class Address:
    ip: IPv4Address
    port: int


class AbstractConnectionInterface(ABC):
    """
    Abstract interface to allow testing the server without a real connection
    """

    @abstractmethod
    def recv(self, size: int) -> bytes:
        raise NotImplementedError

    @abstractmethod
    def send(self, data: bytes) -> None:
        raise NotImplementedError

    @abstractmethod
    def bind(self, addr: Address) -> None:
        raise NotImplementedError

    @abstractmethod
    def close(self) -> None:
        raise NotImplementedError

    @abstractmethod
    def accept(self) -> 'IncomingConnection':
        raise NotImplementedError

    @abstractmethod
    def listen(self, num_connections: int) -> None:
        raise NotImplementedError


@dataclass(frozen=True)
class IncomingConnection:
    connection: AbstractConnectionInterface
    addr: Address


class SocketConnectionInterface(AbstractConnectionInterface):
    def __init__(self, s: socket) -> None:
        super().__init__()
        self.socket = s

    def recv(self, size: int) -> bytes:
        return self.socket.recv(size)

    def send(self, data: bytes) -> None:
        self.socket.sendall(data)

    def close(self) -> None:
        return self.socket.close()

    def accept(self) -> IncomingConnection:
        client_socket, addr = self.socket.accept()
        return IncomingConnection(SocketConnectionInterface(client_socket), addr)

    def bind(self, addr: Address) -> None:
        self.socket.bind((addr.ip, addr.port))

    def listen(self, num_connections: int) -> None:
        self.socket.listen(num_connections)
