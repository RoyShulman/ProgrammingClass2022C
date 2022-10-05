import struct
from enum import Enum
from abc import ABC, abstractmethod
from uuid import UUID
from typing import Any


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


class ClientMessageCodeMissmatch(Exception):
    pass


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

    @staticmethod
    def read_fmt(fmt: str, reader: AbstractClientMessageReader) -> Any:
        return struct.unpack(fmt, reader.read(struct.calcsize(fmt)))

    def validate_message_code(self, expected: ClientMessageCode, real: ClientMessageCode):
        if expected != real:
            raise ClientMessageCodeMissmatch(expected, real)


class ClientMessageHeader(ClientMessage):
    def __init__(self, client_id: UUID, client_version: int, code: ClientMessageCode, payload_size: int) -> None:
        self.client_id = client_id
        self.client_version = client_version
        self.code = code
        self.payload_size = payload_size

    @classmethod
    def unpack(cls, reader: AbstractClientMessageReader) -> 'ClientMessageHeader':
        client_id, client_version, code, payload_size = cls.read_fmt(cls.MESSAGE_FMT(),
                                                                     reader)
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

    def __ne__(self, other: object) -> bool:
        return not self.__eq__(other)


class ClientRegistrationRequest(ClientMessage):
    def __init__(self, name: str) -> None:
        self.name = name

    @classmethod
    def unpack(cls, reader: AbstractClientMessageReader) -> 'ClientRegistrationRequest':
        name = cls.read_fmt(cls.MESSAGE_FMT(), reader)[0]
        return cls(name.decode())

    @staticmethod
    def MESSAGE_FMT() -> str:
        return "<255s"

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, ClientRegistrationRequest):
            return False
        return self.name == other.name

    def __ne__(self, other: object) -> bool:
        return not self.__eq__(other)


class ClientPublicKeyMessage(ClientMessage):
    def __init__(self, name: str, public_key: bytes) -> None:
        self.name = name
        self.public_key = public_key

    @classmethod
    def unpack(cls, reader: AbstractClientMessageReader) -> 'ClientPublicKeyMessage':
        name, public_key = cls.read_fmt(cls.MESSAGE_FMT(), reader)
        return cls(name.decode(), public_key)

    @staticmethod
    def MESSAGE_FMT() -> str:
        return "<255s160s"

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, ClientPublicKeyMessage):
            return False
        return self.name == other.name and self.public_key == other.public_key

    def __ne__(self, other: object) -> bool:
        return not self.__eq__(other)


class UploadFileMessage(ClientMessage):
    def __init__(self, client_uuid: UUID, filename: str, content: bytes) -> None:
        self.client_uuid = client_uuid
        self.filename = filename
        self.content = content

    @classmethod
    def unpack(cls, reader: AbstractClientMessageReader) -> 'UploadFileMessage':
        client_uuid, content_size, filename = cls.read_fmt(
            cls.MESSAGE_FMT(), reader)
        content = cls.read_fmt(f"<{content_size}s", reader)[0]
        return cls(UUID(bytes=client_uuid), filename.decode(), content)

    @staticmethod
    def MESSAGE_FMT() -> str:
        return "<16sI255s"

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, UploadFileMessage):
            return False
        return self.client_uuid == other.client_uuid and self.filename == other.filename and self.content == other.content

    def __ne__(self, other: object) -> bool:
        return not self.__eq__(other)