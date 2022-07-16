import struct
from abc import ABC, abstractmethod
from enum import Enum
from dataclasses import dataclass
from typing import Any, Tuple

from backup_client.response_reader import ResponseReader


@dataclass(frozen=True)
class FilePayload:
    payload: bytes
    size: int


class RequestOP(Enum):
    BACKUP_FILE = 100

    RESTORE_FILE = 200
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
        raise NotImplementedError

    def pack_header(self) -> bytes:
        return struct.pack(self.COMMON_HEADER, self.user_id, self.version, self.request_op.value)

    def pack_filename(self, filename: str) -> bytes:
        fmt = "<H{}s".format(len(filename))
        return struct.pack(fmt, len(filename), filename.encode())

    def pack_payload(self, payload: bytes) -> bytes:
        fmt = "<I{}s".format(len(payload.decode()))
        return struct.pack(fmt, len(payload.decode()), payload)

    def pack_filename_request(self, filename: str) -> bytes:
        return self.pack_header() + self.pack_filename(filename)

    def pack_payload_request(self, filename: str, payload: bytes) -> bytes:
        return self.pack_filename_request(filename) + self.pack_payload(payload)


class ListFilesRequest(ProtocolRequest):
    def __init__(self, user_id: int, version: int) -> None:
        super().__init__(RequestOP.LIST_FILES, user_id, version)

    def pack(self) -> bytes:
        return self.pack_header()


class BackupFileRequest(ProtocolRequest):
    def __init__(self, user_id: int, version: int, filename: str, payload: bytes) -> None:
        super().__init__(RequestOP.BACKUP_FILE, user_id, version)
        self.filename = filename
        self.payload = payload

    def pack(self) -> bytes:
        return self.pack_payload_request(self.filename, self.payload)


class RestoreFileRequest(ProtocolRequest):
    def __init__(self, user_id: int, version: int, filename: str) -> None:
        super().__init__(RequestOP.RESTORE_FILE, user_id, version)
        self.filename = filename

    def pack(self) -> bytes:
        return self.pack_filename_request(self.filename)


class DeleteFileRequest(ProtocolRequest):
    def __init__(self, user_id: int, version: int, filename: str) -> None:
        super().__init__(RequestOP.DELETE_FILE, user_id, version)
        self.filename = filename

    def pack(self) -> bytes:
        return self.pack_filename_request(self.filename)


class ResponseOP(Enum):
    SUCCESSFUL_RESTORE = 210
    SUCCESSFUL_LIST_FILES = 211
    SUCCESSFUL_BACKUP_OR_DELETE = 212

    FILE_NOT_FOUND = 1001
    NO_BACKUP_FILES_FOR_CLIENT = 1002
    SERVER_ERROR = 1003


@dataclass
class VersionedResponse:
    version: int
    response: ResponseOP


class ProtocolResponse(ABC):
    # version - status
    COMMON_HEADER = "<BH"

    def __init__(self, response_op: ResponseOP, version: int) -> None:
        self.response_op = response_op
        self.version = version

    @staticmethod
    def unpack_ver_op(reader: ResponseReader) -> VersionedResponse:
        version, response_op = ProtocolResponse.read_fmt_from_reader(
            ProtocolResponse.COMMON_HEADER, reader)
        return VersionedResponse(version, ResponseOP(response_op))

    @classmethod
    @abstractmethod
    def unpack(cls, version: int, reader: ResponseReader) -> 'ProtocolResponse':
        raise NotImplementedError

    @staticmethod
    def read_fmt_from_reader(fmt: str, reader: ResponseReader) -> Tuple[Any, ...]:
        return struct.unpack(fmt, reader.read(struct.calcsize(fmt)))

    @abstractmethod
    def is_error(self) -> bool:
        raise NotImplementedError

    @classmethod
    def unpack_filename(cls, reader: ResponseReader) -> str:
        name_len = cls.read_fmt_from_reader("<H", reader)[0]
        filename = cls.read_fmt_from_reader(f"<{name_len}s", reader)[0]
        return filename.decode()

    @classmethod
    def unpack_payload(cls, reader: ResponseReader) -> bytes:
        size = cls.read_fmt_from_reader("<I", reader)[0]
        payload = cls.read_fmt_from_reader(f"<{size}s", reader)[0]
        return payload

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, ProtocolResponse):
            return False
        return self.response_op == other.response_op and self.version == other.version

    def __ne__(self, other: object) -> bool:
        return not self == other

    def __str__(self) -> str:
        return self.__class__.__name__


class SuccessfulRestoreResponse(ProtocolResponse):
    def __init__(self, version: int, filename: str, payload: bytes) -> None:
        super().__init__(ResponseOP.SUCCESSFUL_RESTORE, version)
        self.filename = filename
        self.payload = payload

    def is_error(self) -> bool:
        return False

    @classmethod
    def unpack(cls, version: int, reader: ResponseReader) -> 'SuccessfulRestoreResponse':
        filename = cls.unpack_filename(reader)
        payload = cls.unpack_payload(reader)
        return cls(version, filename, payload)


class SuccessfulBackupOrDeleteResponse(ProtocolResponse):
    def __init__(self, version: int, filename: str) -> None:
        super().__init__(ResponseOP.SUCCESSFUL_BACKUP_OR_DELETE, version)
        self.filename = filename

    @classmethod
    def unpack(cls, version: int, reader: ResponseReader) -> 'SuccessfulBackupOrDeleteResponse':
        filename = cls.unpack_filename(reader)
        return cls(version, filename)

    def is_error(self) -> bool:
        return False


class ListFilesResponse(ProtocolResponse):
    def __init__(self, version: int, filename: str, payload: bytes) -> None:
        super().__init__(ResponseOP.SUCCESSFUL_LIST_FILES, version)
        self.name_len = len(filename)
        self.filename = filename
        self.size = len(payload)
        self.payload = payload

    @classmethod
    def unpack(cls, version: int, reader: ResponseReader) -> 'ListFilesResponse':
        filename = cls.unpack_filename(reader)
        payload = cls.unpack_payload(reader)
        return cls(version, filename, payload)

    def is_error(self) -> bool:
        return False

    def __eq__(self, other: object) -> bool:
        if not super().__eq__(other):
            return False

        if not isinstance(other, ListFilesResponse):
            return False
        return self.filename == other.filename and self.payload == other.payload

    def __ne__(self, other: object) -> bool:
        return not self == other


class NoBackupFilesForClientResponse(ProtocolResponse):
    def __init__(self, version: int) -> None:
        super().__init__(ResponseOP.NO_BACKUP_FILES_FOR_CLIENT, version)

    @classmethod
    def unpack(cls, version: int, reader: ResponseReader) -> 'NoBackupFilesForClientResponse':
        return cls(version)

    def is_error(self) -> bool:
        return True


class FileNotFoundResponse(ProtocolResponse):
    def __init__(self, version: int, filename: str) -> None:
        super().__init__(ResponseOP.FILE_NOT_FOUND, version)
        self.filename = filename

    @classmethod
    def unpack(cls, version: int, reader: ResponseReader) -> 'FileNotFoundResponse':
        filename = cls.unpack_filename(reader)
        return cls(version, filename)

    def is_error(self) -> bool:
        return True


class ServerErrorResponse(ProtocolResponse):
    def __init__(self, version: int) -> None:
        super().__init__(ResponseOP.SERVER_ERROR, version)

    @classmethod
    def unpack(cls, version: int, reader: ResponseReader) -> 'ServerErrorResponse':
        return cls(version)

    def is_error(self) -> bool:
        return True


class FailedToParseMessageException(Exception):
    pass


class ResponseParser:
    RESPONSE_OP_TO_CLS = {
        ResponseOP.SUCCESSFUL_RESTORE: SuccessfulRestoreResponse,
        ResponseOP.SUCCESSFUL_LIST_FILES: ListFilesResponse,
        ResponseOP.SUCCESSFUL_BACKUP_OR_DELETE: SuccessfulBackupOrDeleteResponse,

        ResponseOP.FILE_NOT_FOUND: FileNotFoundResponse,
        ResponseOP.NO_BACKUP_FILES_FOR_CLIENT: NoBackupFilesForClientResponse,
        ResponseOP.SERVER_ERROR: ServerErrorResponse,
    }

    @staticmethod
    def parse_message(reader: ResponseReader) -> ProtocolResponse:
        try:
            response = ProtocolResponse.unpack_ver_op(reader)
            return ResponseParser.RESPONSE_OP_TO_CLS[response.response].unpack(response.version, reader)
        except Exception as e:
            raise FailedToParseMessageException(str(e)) from e
