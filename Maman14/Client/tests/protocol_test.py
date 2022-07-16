import unittest
import struct
from io import BytesIO

from backup_client.response_reader import ResponseReader
from backup_client.protocol import (
    ProtocolResponse,
    ResponseParser,
    RequestOP, ResponseOP,
    ListFilesRequest, ListFilesResponse,
    BackupFileRequest, SuccessfulBackupOrDeleteResponse,
    RestoreFileRequest, SuccessfulRestoreResponse,
    DeleteFileRequest,
    NoBackupFilesForClientResponse, FileNotFoundResponse, ServerErrorResponse,
    FailedToParseMessageException
)


class MockResponseReader(ResponseReader):
    def __init__(self, data: bytes) -> None:
        self.buffer = BytesIO(data)
        self.num_reads = 0

    def read(self, size: int) -> bytes:
        self.num_reads += 1
        return self.buffer.read(size)


def get_request_header(user_id: int, version: int, request_op: RequestOP) -> bytes:
    fmt = "<IBB"
    return struct.pack(fmt, user_id, version, request_op.value)


def get_packed_filename(filename: str) -> bytes:
    fmt = f"<H{len(filename)}s"
    return struct.pack(fmt, len(filename), filename.encode())


def get_packed_payload(payload: bytes) -> bytes:
    fmt = f"<I{len(payload.decode())}s"
    return struct.pack(fmt, len(payload), payload)


def get_request_filename(user_id: int, version: int, request_op: RequestOP, filename: str) -> bytes:
    header = get_request_header(user_id, version, request_op)
    packed_filename = get_packed_filename(filename)
    return header + packed_filename


def get_request_filename_payload(user_id: int, version: int, request_op: RequestOP, filename: str, payload: bytes) -> bytes:
    header_filename = get_request_filename(user_id,
                                           version,
                                           request_op,
                                           filename)
    packed_payload = get_packed_payload(payload)
    return header_filename + packed_payload


def get_response_header(version: int, status: ResponseOP) -> bytes:
    fmt = "<BH"
    return struct.pack(fmt, version, status.value)


def get_response_filename(version: int, status: ResponseOP, filename: str) -> bytes:
    header = get_response_header(version, status)
    packed_filename = get_packed_filename(filename)
    return header + packed_filename


def get_response_filename_payload(version: int, status: ResponseOP, filename: str, payload: bytes) -> bytes:
    header_filename = get_response_filename(version, status, filename)
    packed_payload = get_packed_payload(payload)
    return header_filename + packed_payload


class ProtocolTest(unittest.TestCase):
    def common_response_test(self, num_reads: int, is_error: bool, expected: ProtocolResponse,
                             actual: bytes):
        response_reader = MockResponseReader(actual)
        actual_response = ResponseParser.parse_message(response_reader)
        self.assertEqual(expected, actual_response)

        if is_error:
            self.assertTrue(actual_response.is_error())
        else:
            self.assertFalse(actual_response.is_error())
        self.assertEqual(num_reads, response_reader.num_reads)

    def test_list_files_request(self):
        user_id = 1
        version = 20
        request = ListFilesRequest(user_id, version).pack()
        expected = get_request_header(user_id, version, RequestOP.LIST_FILES)
        self.assertEqual(expected, request)

    def test_list_files_response(self):
        version = 20
        filename = "coolname"
        payload = "firstpath".encode()
        expected = ListFilesResponse(version, filename, payload)

        actual = get_response_filename_payload(version,
                                               ResponseOP.SUCCESSFUL_LIST_FILES,
                                               filename,
                                               payload)

        self.common_response_test(5, False, expected, actual)

    def test_invalid_response(self):
        with self.assertRaises(FailedToParseMessageException):
            ResponseParser.parse_message(
                MockResponseReader("aaaaaaaa".encode()))

    def test_no_backup_files_for_client(self):
        version = 123
        expected = NoBackupFilesForClientResponse(version)
        actual = get_response_header(version,
                                     ResponseOP.NO_BACKUP_FILES_FOR_CLIENT)

        self.common_response_test(1, True, expected, actual)

    def test_backup_file_request(self):
        user_id = 421
        version = 23
        filename = "myfile"
        payload = "supercoolpayload".encode()
        request = BackupFileRequest(user_id, version, filename, payload)

        expected = get_request_filename_payload(user_id,
                                                version,
                                                RequestOP.BACKUP_FILE,
                                                filename,
                                                payload)
        self.assertEqual(expected, request.pack())

    def test_successful_backup_or_delete_file_response(self):
        version = 112
        filename = "verycoolfilename.txt"
        expected = SuccessfulBackupOrDeleteResponse(version, filename)

        actual = get_response_filename(version,
                                       ResponseOP.SUCCESSFUL_BACKUP_OR_DELETE,
                                       filename)

        self.common_response_test(3, False, expected, actual)

    def test_restore_file_request(self):
        version = 111
        user_id = 11223
        filename = "nicefile.pdf"
        request = RestoreFileRequest(user_id, version, filename)

        expected = get_request_filename(user_id,
                                        version,
                                        RequestOP.RESTORE_FILE,
                                        filename)
        self.assertEqual(expected, request.pack())

    def test_successful_restore_response(self):
        version = 99
        filename = "supercoolfile.txt"
        payload = "thisisthepayload\nyep\n".encode()
        expected = SuccessfulRestoreResponse(version, filename, payload)

        actual = get_response_filename_payload(version,
                                               ResponseOP.SUCCESSFUL_RESTORE,
                                               filename,
                                               payload)
        self.common_response_test(5, False, expected, actual)

    def test_delete_file_request(self):
        version = 21
        user_id = 2222111
        filename = "deletethis.png"
        request = DeleteFileRequest(user_id, version, filename)

        expected = get_request_filename(
            user_id, version, RequestOP.DELETE_FILE, filename)
        self.assertEqual(expected, request.pack())

    def test_file_not_found_response(self):
        version = 17
        filename = "notfound.py"

        expected = FileNotFoundResponse(version, filename)

        actual = get_response_filename(version,
                                       ResponseOP.FILE_NOT_FOUND,
                                       filename)
        self.common_response_test(3, True, expected, actual)

    def test_server_error_response(self):
        version = 15
        expected = ServerErrorResponse(version)

        actual = get_response_header(version,
                                     ResponseOP.SERVER_ERROR)
        self.common_response_test(1, True, expected, actual)


if __name__ == "__main__":
    unittest.main()
