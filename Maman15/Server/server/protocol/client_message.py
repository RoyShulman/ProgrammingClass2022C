import struct
from enum import Enum
from abc import ABC, abstractmethod
from uuid import UUID


class ClientMessageCode(Enum):
    REGISTRATION = 1100
    CLIENT_PUBLIC_KEY = 1101
    UPLOAD_FILE = 1103
    FILE_CRC_OK = 1104
    CRC_INCORRECT_WILL_RETRY = 1105
    CRC_INCORRECT_GIVING_UP = 1106


class AbstractClientMessageReader(ABC):
    """
    Abstract class to read messages.
    Allows us to test the ClientMessage parsing without a connection
    """
    @abstractmethod
    def read(self, size: int) -> bytes:
        raise NotImplementedError


class ClientMessage(ABC):
    """
    Base class for all client messages.
    The class should support unpacking from a AbstractClientMessageReader
    """

    @classmethod
    @abstractmethod
    def unpack(cls, reader: AbstractClientMessageReader) -> 'ClientMessage':
        raise NotImplementedError

    @staticmethod
    @abstractmethod
    def MESSAGE_FMT() -> str:
        raise NotImplementedError


class ClientMessageHeader(ClientMessage):
    def __init__(self, client_id: UUID, client_version: int, code: ClientMessageCode, payload_size: int) -> None:
        self.client_id = client_id
        self.client_version = client_version
        self.code = code
        self.payload_size = payload_size

    @classmethod
    def unpack(cls, reader: AbstractClientMessageReader) -> 'ClientMessageHeader':
        client_id, client_version, code, payload_size = struct.unpack(cls.MESSAGE_FMT(),
                                                                      reader.read(struct.calcsize(cls.MESSAGE_FMT())))
        return cls(UUID(bytes=client_id), client_version, ClientMessageCode(code), payload_size)

    @staticmethod
    def MESSAGE_FMT() -> str:
        return "<16sBHI"

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, ClientMessageHeader):
            return False
        return self.client_id == other.client_id and \
            self.client_version == other.client_version and \
            self.code == other.code and \
            self.payload_size == other.payload_size
