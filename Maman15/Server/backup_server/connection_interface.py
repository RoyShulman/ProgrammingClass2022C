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
    """
    Implementation for AbstractConnectionInterface using a tcp socket
    """

    def __init__(self, s: socket) -> None:
        super().__init__()
        self.socket = s

    def recv(self, size: int) -> bytes:
        """
        recv bytes from the socket of the given size
        """
        return self.socket.recv(size)

    def send(self, data: bytes) -> None:
        """
        Send the given data using the socket
        """
        self.socket.sendall(data)

    def close(self) -> None:
        """
        Close the connection
        """
        return self.socket.close()

    def accept(self) -> IncomingConnection:
        """
        Accept an incoming connection from the network
        """
        client_socket, addr = self.socket.accept()
        return IncomingConnection(SocketConnectionInterface(client_socket), addr)

    def bind(self, addr: Address) -> None:
        """
        Bind the socket to the given address
        """
        self.socket.bind((str(addr.ip), addr.port))

    def listen(self, num_connections: int) -> None:
        """
        Listen for the number of connections given
        """
        self.socket.listen(num_connections)
