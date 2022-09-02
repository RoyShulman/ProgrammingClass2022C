import unittest
import struct
from backup_client.protocol import (
    ResponseParser,
    RequestOP, ResponseOP,
    ListFilesRequest, ListFilesResponse
)


class ProtocolTest(unittest.TestCase):
    def test_list_files_request(self):
        user_id = 1
        version = 20
        request = ListFilesRequest(user_id, version).pack()
        expected = struct.pack("<IBB", user_id, version,
                               RequestOP.LIST_FILES.value)
        self.assertEqual(expected, request)

    def test_list_files_response(self):
        version = 20
        filename = "coolname"
        payload = "firstpath"
        expected = ListFilesResponse(version, filename, payload.encode())
        actual = struct.pack(f"<BHH{len(filename)}sI{len(payload)}s", version, ResponseOP.SUCCESSFUL_LIST_FILES.value,
                             len(filename), filename.encode(), len(payload), payload.encode())
        self.assertEqual(expected, ResponseParser.parse_message(actual))


if __name__ == "__main__":
    unittest.main()
