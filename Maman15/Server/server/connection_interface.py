from abc import ABC
from socket import socket


class AbstractConnectionInterface(ABC):
    """
    Abstract interface to allow testing the server without a real connection
    """

    def recv(self, size: int) -> bytes:
        raise NotImplementedError

    def send(self, data: bytes) -> None:
        raise NotImplementedError


class SocketConnectionInterface(AbstractConnectionInterface):
    def __init__(self, client_socket: socket) -> None:
        super().__init__()
        self.client_socket = client_socket

    def recv(self, size: int) -> bytes:
        return self.client_socket.recv(size)

    def send(self, data: bytes) -> None:
        self.client_socket.sendall(data)
