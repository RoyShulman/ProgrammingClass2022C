#include "Maman14/Server/request_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "Maman14/Server/protocol/request.h"
#include "Maman14/Server/request_reader.h"

class MockRequestReader : public AbstractRequestReader {
public:
    MOCK_METHOD0(read_uint32, uint32_t());
    MOCK_METHOD0(read_uint16, uint16_t());
    MOCK_METHOD0(read_uint8, uint8_t());
    MOCK_METHOD1(read_bytes, vector<uint8_t>(size_t));
};

using std::unique_ptr;
using ::testing::Return;
using ::testing::ReturnPointee;

template <typename Derived, typename Base>
inline std::unique_ptr<Derived> dynamic_pointer_cast(std::unique_ptr<Base>&& ptr_) {
    Derived* const converted_ptr = dynamic_cast<Derived*>(ptr_.get());
    if (!converted_ptr) {
        throw std::bad_cast();
    }

    ptr_.release();
    return std::unique_ptr<Derived>(converted_ptr);
}

TEST(RequestTest, list_files_request) {
    MockRequestReader* mock_reader = new MockRequestReader();
    EXPECT_CALL(*mock_reader, read_uint32())
        .WillOnce(Return(123));

    EXPECT_CALL(*mock_reader, read_uint8())
        .WillOnce(Return(1))
        .WillOnce(Return(202));

    unique_ptr<MockRequestReader> reader{mock_reader};
    RequestParser parser(std::move(reader));

    unique_ptr<ListFilesRequest> request{dynamic_pointer_cast<ListFilesRequest>(parser.parse_message(1))};
    ASSERT_EQ(123, request->get_user_id());
}

TEST(RequestTest, invalid_op_request) {
    MockRequestReader* mock_reader = new MockRequestReader();
    EXPECT_CALL(*mock_reader, read_uint32())
        .WillOnce(Return(123));

    EXPECT_CALL(*mock_reader, read_uint8())
        .WillOnce(Return(1))
        .WillOnce(Return(0));

    unique_ptr<MockRequestReader> reader{mock_reader};
    RequestParser parser(std::move(reader));

    EXPECT_THROW(
        {
            unique_ptr<ListFilesRequest> request{dynamic_pointer_cast<ListFilesRequest>(parser.parse_message(1))};
        },
        InvalidRequestException);
}

TEST(RequestTest, wrong_request_cast) {
    MockRequestReader* mock_reader = new MockRequestReader();
    string filename{"file.jpeg"};
    vector<uint8_t> filename_payload(filename.begin(), filename.end());
    EXPECT_CALL(*mock_reader, read_uint32())
        .WillOnce(Return(123));

    EXPECT_CALL(*mock_reader, read_uint8())
        .WillOnce(Return(1))
        .WillOnce(Return(202));

    unique_ptr<MockRequestReader> reader{mock_reader};
    RequestParser parser(std::move(reader));

    EXPECT_THROW(
        {
            unique_ptr<BackupFileRequest> request{dynamic_pointer_cast<BackupFileRequest>(parser.parse_message(1))};
        },
        std::bad_cast);
}

TEST(RequestTest, wrong_version) {
    MockRequestReader* mock_reader = new MockRequestReader();
    EXPECT_CALL(*mock_reader, read_uint32())
        .WillOnce(Return(123));

    EXPECT_CALL(*mock_reader, read_uint8())
        .WillOnce(Return(50));

    unique_ptr<MockRequestReader> reader{mock_reader};
    RequestParser parser(std::move(reader));

    EXPECT_THROW(
        {
            unique_ptr<ListFilesRequest> request{dynamic_pointer_cast<ListFilesRequest>(parser.parse_message(1))};
        },
        VersionMismatchException);
}

TEST(RequestTest, backup_file_request) {
    MockRequestReader* mock_reader = new MockRequestReader();
    string filename{"file.jpeg"};
    vector<uint8_t> filename_vector(filename.begin(), filename.end());

    string payload{"This is the payload\nyes\n"};
    vector<uint8_t> payload_vector(payload.begin(), payload.end());

    EXPECT_CALL(*mock_reader, read_uint32())
        .WillOnce(Return(123))
        .WillOnce(Return(payload_vector.size()));

    EXPECT_CALL(*mock_reader, read_uint8())
        .WillOnce(Return(1))
        .WillOnce(Return(100));

    EXPECT_CALL(*mock_reader, read_uint16())
        .WillOnce(Return(filename.size()));

    EXPECT_CALL(*mock_reader, read_bytes(filename.size()))
        .WillOnce(Return(filename_vector));

    EXPECT_CALL(*mock_reader, read_bytes(payload.size()))
        .WillOnce(Return(payload_vector));

    unique_ptr<MockRequestReader> reader{mock_reader};
    RequestParser parser(std::move(reader));
    unique_ptr<BackupFileRequest> request{dynamic_pointer_cast<BackupFileRequest>(parser.parse_message(1))};

    ASSERT_EQ(123, request->get_user_id());
    ASSERT_EQ(filename, request->get_filename());
    ASSERT_EQ(payload_vector, request->get_payload());
}

TEST(RequestTest, restore_file_request) {
    MockRequestReader* mock_reader = new MockRequestReader();
    string filename{"file.jpeg"};
    vector<uint8_t> filename_vector(filename.begin(), filename.end());

    EXPECT_CALL(*mock_reader, read_uint32())
        .WillOnce(Return(123));

    EXPECT_CALL(*mock_reader, read_uint8())
        .WillOnce(Return(1))
        .WillOnce(Return(200));

    EXPECT_CALL(*mock_reader, read_uint16())
        .WillOnce(Return(filename.size()));

    EXPECT_CALL(*mock_reader, read_bytes(filename.size()))
        .WillOnce(Return(filename_vector));

    unique_ptr<MockRequestReader> reader{mock_reader};
    RequestParser parser(std::move(reader));
    unique_ptr<RestoreFileRequest> request{dynamic_pointer_cast<RestoreFileRequest>(parser.parse_message(1))};

    ASSERT_EQ(123, request->get_user_id());
    ASSERT_EQ(filename, request->get_filename());
}

TEST(RequestTest, delete_file_request) {
    MockRequestReader* mock_reader = new MockRequestReader();
    string filename{"file.jpeg"};
    vector<uint8_t> filename_vector(filename.begin(), filename.end());

    EXPECT_CALL(*mock_reader, read_uint32())
        .WillOnce(Return(123));

    EXPECT_CALL(*mock_reader, read_uint8())
        .WillOnce(Return(1))
        .WillOnce(Return(201));

    EXPECT_CALL(*mock_reader, read_uint16())
        .WillOnce(Return(filename.size()));

    EXPECT_CALL(*mock_reader, read_bytes(filename.size()))
        .WillOnce(Return(filename_vector));

    unique_ptr<MockRequestReader> reader{mock_reader};
    RequestParser parser(std::move(reader));
    unique_ptr<DeleteFileRequest> request{dynamic_pointer_cast<DeleteFileRequest>(parser.parse_message(1))};

    ASSERT_EQ(123, request->get_user_id());
    ASSERT_EQ(filename, request->get_filename());
}
