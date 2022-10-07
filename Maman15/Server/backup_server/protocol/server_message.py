import struct
from enum import Enum
from abc import ABC, abstractmethod
from uuid import UUID


class ServerMessageCode(Enum):
    REGISTRATION_SUCCESSFUL = 2100
    REGISTRATION_FAILED = 2101
    AES_KEY = 2102
    UPLOAD_FILE_SUCCESSFUL = 2103
    SUCCESS_RESPONSE = 2104


class ServerMessage(ABC):
    HEADER_FMT = "<BHI"

    def __init__(self, server_version: int, code: ServerMessageCode) -> None:
        self.server_version = server_version
        self.code = code

    @abstractmethod
    def pack(self) -> bytearray:
        return self.pack_header()

    @property
    @abstractmethod
    def PAYLOAD_FMT(self) -> str:
        raise NotImplementedError

    def get_payload_size(self) -> int:
        return struct.calcsize(self.PAYLOAD_FMT)

    def pack_header(self) -> bytearray:
        return bytearray(struct.pack(self.HEADER_FMT, self.server_version,
                                     self.code.value, self.get_payload_size()))


class RegistrationSuccessfulMessage(ServerMessage):
    def __init__(self, server_version: int, uuid: UUID) -> None:
        super().__init__(server_version, ServerMessageCode.REGISTRATION_SUCCESSFUL)
        self.uuid = uuid

    def pack(self) -> bytearray:
        return super().pack() + struct.pack(self.PAYLOAD_FMT, self.uuid.bytes)

    @property
    def PAYLOAD_FMT(self) -> str:
        return "<16s"


class RegistrationFailedMessage(ServerMessage):
    def __init__(self, server_version: int) -> None:
        super().__init__(server_version, ServerMessageCode.REGISTRATION_FAILED)

    def pack(self) -> bytearray:
        return super().pack()

    @property
    def PAYLOAD_FMT(self) -> str:
        return ""


class AESKeyMessage(ServerMessage):
    def __init__(self, server_version: int, uuid: UUID, aes_key: bytes) -> None:
        super().__init__(server_version, ServerMessageCode.AES_KEY)
        self.uuid = uuid
        self.aes_key = aes_key

    def pack(self) -> bytearray:
        return super().pack() + struct.pack(self.PAYLOAD_FMT, self.uuid.bytes, self.aes_key)

    @property
    def PAYLOAD_FMT(self) -> str:
        return f"<16s{len(self.aes_key)}s"


class UploadFileSuccessfulMessage(ServerMessage):
    def __init__(self, server_version: int, uuid: UUID, content_size: int,
                 filename: str, checksum: int) -> None:
        super().__init__(server_version, ServerMessageCode.UPLOAD_FILE_SUCCESSFUL)
        self.uuid = uuid
        self.content_size = content_size
        self.filename = filename
        self.checksum = checksum

    def pack(self) -> bytearray:
        return super().pack() + struct.pack(self.PAYLOAD_FMT, self.uuid.bytes,
                                            self.content_size, self.filename.encode(), self.checksum)

    @property
    def PAYLOAD_FMT(self) -> str:
        return "<16sI255sI"


class SuccessResponseMessage(ServerMessage):
    def __init__(self, server_version: int) -> None:
        super().__init__(server_version, ServerMessageCode.SUCCESS_RESPONSE)

    def pack(self) -> bytearray:
        return super().pack()

    @property
    def PAYLOAD_FMT(self) -> str:
        return ""
