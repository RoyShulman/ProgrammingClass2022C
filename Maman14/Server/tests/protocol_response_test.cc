#include <gtest/gtest.h>

#include <vector>

#include "Maman14/Server/bytearray.h"
#include "Maman14/Server/protocol/response.h"

using std::string;
using std::vector;
using utils::Bytearray;

#include <iostream>

static Bytearray pack_header(ResponseOP op, ProtocolVersion version) {
    Bytearray packed;
    packed.push_u8(version);
    packed.push_u16(static_cast<uint16_t>(op));
    return packed;
}

static Bytearray pack_filename_response(ResponseOP op, ProtocolVersion version, const string& filename) {
    Bytearray packed = pack_header(op, version);
    packed.push_u16(static_cast<uint16_t>(filename.length()));
    packed.push_string(filename);
    return packed;
}

static Bytearray pack_payload_filename_response(ResponseOP op, ProtocolVersion version, const string& filename,
                                                const Bytearray& payload) {
    Bytearray packed = pack_filename_response(op, version, filename);
    packed.push_u32(payload.len());
    packed.push_bytes(payload);
    return packed;
}

TEST(ProtocolTest, restore_response) {
    ProtocolVersion version{123};
    string filename("coolfile.txt");
    Bytearray payload;
    payload.push_string(string("This is my file\n"));

    SuccessfulRestoreResponse response(123, "coolfile.txt", payload);
    Bytearray packed_response = response.pack();
    Bytearray expected = pack_payload_filename_response(ResponseOP::SUCCESSFUL_RESTORE, version, filename, payload);

    ASSERT_EQ(1 + 2 + 2 + 12 + 4 + 16, expected.len());
    ASSERT_EQ(expected.len(), packed_response.len());
    ASSERT_TRUE(memcmp(expected.data(), packed_response.data(), packed_response.len()) == 0);
}

TEST(ProtocolTest, list_files_response) {
    ProtocolVersion version{123};
    string filename("coolfile.txt");
    Bytearray payload;
    payload.push_string(string("file1.jpeg\nfile2.txt\r\n"));

    SuccessfulListFilesResponse response(123, "coolfile.txt", payload);
    Bytearray packed_response = response.pack();
    Bytearray expected = pack_payload_filename_response(ResponseOP::SUCCESSFUL_LIST_FILES, version, filename, payload);

    ASSERT_EQ(1 + 2 + 2 + filename.size() + 4 + string("file1.jpeg\nfile2.txt\r\n").size(), expected.len());
    ASSERT_EQ(expected.len(), packed_response.len());
    ASSERT_TRUE(memcmp(expected.data(), packed_response.data(), packed_response.len()) == 0);
}

TEST(ProtocolTest, backup_or_delete) {
    ProtocolVersion version{123};
    string filename("coolfile2.txt");

    SuccessfulBackupOrDeleteResponse response(123, filename);
    Bytearray packed_response = response.pack();
    Bytearray expected = pack_filename_response(ResponseOP::SUCCESSFUL_BACKUP_OR_DELETE, version, filename);

    ASSERT_EQ(1 + 2 + 2 + filename.size(), expected.len());
    ASSERT_EQ(expected.len(), packed_response.len());
    ASSERT_TRUE(memcmp(expected.data(), packed_response.data(), packed_response.len()) == 0);
}

TEST(ProtocolTest, file_not_found) {
    ProtocolVersion version{123};
    string filename("coolfile211.txt");

    FileNotFoundResponse response(version, filename);
    Bytearray packed_response = response.pack();
    Bytearray expected = pack_filename_response(ResponseOP::FILE_NOT_FOUND, version, filename);

    ASSERT_EQ(1 + 2 + 2 + filename.size(), expected.len());
    ASSERT_EQ(expected.len(), packed_response.len());
    ASSERT_TRUE(memcmp(expected.data(), packed_response.data(), packed_response.len()) == 0);
}

TEST(ProtocolTest, no_back_files_for_client) {
    ProtocolVersion version{123};

    NoBackupFilesForClientResponse response(version);
    Bytearray packed_response = response.pack();
    Bytearray expected = pack_header(ResponseOP::NO_BACKUP_FILES_FOR_CLIENT, version);

    ASSERT_EQ(1 + 2, expected.len());
    ASSERT_EQ(expected.len(), packed_response.len());
    ASSERT_TRUE(memcmp(expected.data(), packed_response.data(), packed_response.len()) == 0);
}

TEST(ProtocolTest, server_error) {
    ProtocolVersion version{123};

    ServerErrorResponse response(version);
    Bytearray packed_response = response.pack();
    Bytearray expected = pack_header(ResponseOP::SERVER_ERROR, version);

    ASSERT_EQ(1 + 2, expected.len());
    ASSERT_EQ(expected.len(), packed_response.len());
    ASSERT_TRUE(memcmp(expected.data(), packed_response.data(), packed_response.len()) == 0);
}
