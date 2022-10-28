import struct
from enum import Enum
from abc import ABC, abstractmethod
from uuid import UUID
from typing import Any
from dataclasses import dataclass
from backup_server.connection_interface import AbstractConnectionInterface


class ClientMessageCode(Enum):
    REGISTRATION = 1100
    CLIENT_PUBLIC_KEY = 1101
    UPLOAD_FILE = 1103
    FILE_CRC_OK = 1104
    CRC_INCORRECT_WILL_RETRY = 1105
    CRC_INCORRECT_GIVING_UP = 1106


class ClientMessageReader:
    """
    class to read messages.
    Allows us to test the ClientMessage parsing without a connection
    """

    def __init__(self, connection: AbstractConnectionInterface) -> None:
        self.connection = connection

    def read(self, size: int) -> bytes:
        """
        Read the given size from the connection
        """
        return self.connection.recv(size)


class ClientMessageCodeMissmatch(Exception):
    """
    Exception raised when we expect a specific code from the client
    but are sent a different one
    """
    pass


class ClientMessage(ABC):
    """
    Base class for all client messages.
    The class should support unpacking from a ClientMessageReader
    """
    @classmethod
    @abstractmethod
    def unpack(cls, reader: ClientMessageReader) -> 'ClientMessage':
        """
        Subclasses should override this method and unpack their class using the reader
        """
        raise NotImplementedError

    @staticmethod
    @abstractmethod
    def MESSAGE_FMT() -> str:
        """
        All subclasses should override this method and specify the expected message format
        """
        raise NotImplementedError

    @staticmethod
    def read_fmt(fmt: str, reader: ClientMessageReader) -> Any:
        """
        Read from the ClientMessageReader and unpack the message according to the format
        """
        return struct.unpack(fmt, reader.read(struct.calcsize(fmt)))

    def validate_message_code(self, expected: ClientMessageCode, real: ClientMessageCode):
        """
        Raises an exception if the expected code doesn't match the real parsed code
        """
        if expected != real:
            raise ClientMessageCodeMissmatch(expected, real)


class ClientMessageHeader(ClientMessage):
    def __init__(self, client_id: UUID, client_version: int, code: ClientMessageCode, payload_size: int) -> None:
        self.client_id = client_id
        self.client_version = client_version
        self.code = code
        self.payload_size = payload_size

    @classmethod
    def unpack(cls, reader: ClientMessageReader) -> 'ClientMessageHeader':
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
    def unpack(cls, reader: ClientMessageReader) -> 'ClientRegistrationRequest':
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
    def unpack(cls, reader: ClientMessageReader) -> 'ClientPublicKeyMessage':
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
    def unpack(cls, reader: ClientMessageReader) -> 'UploadFileMessage':
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


class FileCRCOKMessage(ClientMessage):
    def __init__(self, client_uuid: UUID, filename: str) -> None:
        self.client_uuid = client_uuid
        self.filename = filename

    @classmethod
    def unpack(cls, reader: ClientMessageReader) -> 'FileCRCOKMessage':
        client_uuid, filename = cls.read_fmt(cls.MESSAGE_FMT(), reader)
        return cls(client_uuid, filename.decode())

    @staticmethod
    def MESSAGE_FMT() -> str:
        return "<16s255s"


class FileCRCIncorrectWillRetryMessage(ClientMessage):
    def __init__(self, client_uuid: UUID, filename: str) -> None:
        self.client_uuid = client_uuid
        self.filename = filename

    @classmethod
    def unpack(cls, reader: ClientMessageReader) -> 'FileCRCIncorrectWillRetryMessage':
        client_uuid, filename = cls.read_fmt(cls.MESSAGE_FMT(), reader)
        return cls(client_uuid, filename.decode())

    @staticmethod
    def MESSAGE_FMT() -> str:
        return "<16s255s"


class FileCRCIncorrectGivingUpMessage(ClientMessage):
    def __init__(self, client_uuid: UUID, filename: str) -> None:
        self.client_uuid = client_uuid
        self.filename = filename

    @classmethod
    def unpack(cls, reader: ClientMessageReader) -> 'FileCRCIncorrectGivingUpMessage':
        client_uuid, filename = cls.read_fmt(cls.MESSAGE_FMT(), reader)
        return cls(client_uuid, filename.decode())

    @staticmethod
    def MESSAGE_FMT() -> str:
        return "<16s255s"


class FailedToParseMessage(Exception):
    """
    Exception raised when we fail to parse a message from the client
    """
    pass


@dataclass(frozen=True)
class ClientMessageWithHeader:
    """
    All client messages have a header and a payload
    """
    header: ClientMessageHeader
    payload: ClientMessage


class ClientMessageParser:
    MESSAGE_CODE_TO_CLS = {
        ClientMessageCode.REGISTRATION: ClientRegistrationRequest,
        ClientMessageCode.CLIENT_PUBLIC_KEY: ClientPublicKeyMessage,
        ClientMessageCode.UPLOAD_FILE: UploadFileMessage,
        ClientMessageCode.FILE_CRC_OK: FileCRCOKMessage,
        ClientMessageCode.CRC_INCORRECT_WILL_RETRY: FileCRCIncorrectWillRetryMessage,
        ClientMessageCode.CRC_INCORRECT_GIVING_UP: FileCRCIncorrectGivingUpMessage,
    }

    @staticmethod
    def parse_message(reader: ClientMessageReader) -> 'ClientMessageWithHeader':
        """
        Parse client messages according to their code, and return their class representation
        """
        header = ClientMessageHeader.unpack(reader)
        try:
            payload = ClientMessageParser.MESSAGE_CODE_TO_CLS[header.code].unpack(
                reader)
            return ClientMessageWithHeader(header, payload)
        except Exception as e:
            raise FailedToParseMessage(header.code) from e
