import struct
from abc import ABC, abstractclassmethod
from enum import Enum
from dataclasses import dataclass


@dataclass(frozen=True)
class FilePayload:
    payload: bytes
    size: int


class MessageOp(Enum):
    SAVE_FILE = 100
    GET_FILE = 200
    DELETE_FILE = 201
    LIST_FILES = 202


class ProtocolRequest(ABC):
    COMMON_HEADER = "<IBB"

    def __init__(self, message_op: MessageOp, user_id: int, version: int) -> None:
        super().__init__()
        self.message_op = message_op
        self.user_id = user_id
        self.version = version

    @abstractclassmethod
    def serialize(self) -> bytes:
        return struct.pack(self.COMMON_HEADER, self.user_id, self.version, self.message_op)


class SaveFileRequest(ProtocolRequest):
    MESSAGE_FMT = "<H{filename_length}sI{filesize}s"

    def __init__(self, user_id: int, version: int, filename: str) -> None:
        super().__init__(MessageOp.SAVE_FILE, user_id, version)
        self.filename = filename
        self.filename_length = len(filename)
        self.file_payload = self.get_file_payload()

    def serialize(self) -> bytes:
        common = super().serialize()
        exact_length_request = self.get_message_fmt_with_sizes()
        return common + struct.pack(exact_length_request,
                                    self.filename_length, self.filename,
                                    self.file_payload.size, self.file_payload.payload)

    def get_message_fmt_with_sizes(self):
        return self.MESSAGE_FMT.format(filename_length=self.filename_length,
                                       filesize=self.file_payload.size)

    def get_file_payload(self) -> FilePayload:
        with open(self.filename, "rb") as f:
            payload = f.read()
        return FilePayload(payload, len(payload))
