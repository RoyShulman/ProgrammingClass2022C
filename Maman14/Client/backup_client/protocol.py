import struct
from abc import ABC, abstractmethod
from enum import Enum
from dataclasses import dataclass
from io import BytesIO
from typing import Any, Tuple


@dataclass(frozen=True)
class FilePayload:
    payload: bytes
    size: int


class RequestOP(Enum):
    SAVE_FILE = 100
    GET_FILE = 200
    DELETE_FILE = 201
    LIST_FILES = 202


class ProtocolRequest(ABC):
    # userID - version - op
    COMMON_HEADER = "<IBB"

    def __init__(self, request_op: RequestOP, user_id: int, version: int) -> None:
        super().__init__()
        self.request_op = request_op
        self.user_id = user_id
        self.version = version

    @abstractmethod
    def pack(self) -> bytes:
        return struct.pack(self.COMMON_HEADER, self.user_id, self.version, self.request_op.value)


class ListFilesRequest(ProtocolRequest):
    def __init__(self, user_id: int, version: int) -> None:
        super().__init__(RequestOP.LIST_FILES, user_id, version)

    def pack(self) -> bytes:
        return super().pack()


class ResponseOP(Enum):
    SUCCESSFUL_BACKUP = 201
    SUCCESSFUL_LIST_FILES = 211


@dataclass
class VersionedResponse:
    version: int
    response: ResponseOP


class ProtocolResponse(ABC):
    # version - status
    COMMON_HEADER = "<BH"

    def __init__(self, response_op: ResponseOP, version: int) -> None:
        self.response_op = response_op

    @staticmethod
    def unpack_ver_op(buffer: BytesIO) -> VersionedResponse:
        version, response_op = ProtocolResponse.read_fmt_from_buffer(
            ProtocolResponse.COMMON_HEADER, buffer)
        return VersionedResponse(version, ResponseOP(response_op))

    @classmethod
    @abstractmethod
    def unpack(cls, version: int, buffer: BytesIO) -> 'ProtocolResponse':
        raise NotImplementedError

    @staticmethod
    def read_fmt_from_buffer(fmt: str, buffer: BytesIO) -> Tuple[Any, ...]:
        return struct.unpack(fmt, buffer.read(struct.calcsize(fmt)))


class ListFilesResponse(ProtocolResponse):
    def __init__(self, version: int, filename: str, payload: bytes) -> None:
        super().__init__(ResponseOP.SUCCESSFUL_LIST_FILES, version)
        self.name_len = len(filename)
        self.filename = filename
        self.size = len(payload)
        self.payload = payload

    @classmethod
    def unpack(cls, version: int, buffer: BytesIO) -> 'ListFilesResponse':
        name_len = cls.read_fmt_from_buffer("<H", buffer)[0]
        filename = cls.read_fmt_from_buffer(f"<{name_len}s", buffer)[0]
        size = cls.read_fmt_from_buffer("<I", buffer)[0]
        payload = cls.read_fmt_from_buffer(f"<{size}s", buffer)[0]
        return cls(version, filename.decode(), payload)

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, ListFilesResponse):
            return False
        return self.filename == other.filename and self.payload == other.payload

    def __ne__(self, other: object) -> bool:
        return not self == other


class FailedToParseMessage(Exception):
    pass


class ResponseParser:
    RESPONSE_OP_TO_CLS = {
        ResponseOP.SUCCESSFUL_LIST_FILES: ListFilesResponse
    }

    @staticmethod
    def parse_message(message: bytes) -> ProtocolResponse:
        buffer = BytesIO(message)
        try:
            response = ProtocolResponse.unpack_ver_op(buffer)
            return ResponseParser.RESPONSE_OP_TO_CLS[response.response].unpack(response.version, buffer)
        except Exception as e:
            raise FailedToParseMessage(str(e))
