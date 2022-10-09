#include "Maman15/Client/protocol/server_message.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/uuid/string_generator.hpp>

#include "Maman15/Client/incoming_message_reader.h"

class MockIncomingMessageReader : public client::AbstractIncomingMessageReader {
public:
    MOCK_METHOD0(read_uint32, uint32_t());
    MOCK_METHOD0(read_uint16, uint16_t());
    MOCK_METHOD0(read_uint8, uint8_t());
    MOCK_METHOD1(read_bytes, string(size_t));
};

using client::protocol::AESKeyMessage;
using client::protocol::RegistrationSuccessfulMessage;
using client::protocol::SuccessResponseMessage;
using client::protocol::UploadFileSuccessfulMessage;
using ::testing::Return;

TEST(ServerMessageTest, registration_successful_message) {
    MockIncomingMessageReader* mock_reader = new MockIncomingMessageReader;
    EXPECT_CALL(*mock_reader, read_uint8())
        .WillOnce(Return(3));
    EXPECT_CALL(*mock_reader, read_uint16())
        .WillOnce(Return(2100));
    EXPECT_CALL(*mock_reader, read_bytes(16))
        .WillOnce(Return(string{'\xbb', '\x4f', '\x44', '\x60', '\x3c', '\x25', '\x11', '\xed', '\xb2', '\xbb',
                                '\xf2', '\xd2', '\x0d', '\xd2', '\x44', '\x80'}));
    EXPECT_CALL(*mock_reader, read_uint32())
        .WillOnce(Return(16));

    shared_ptr<MockIncomingMessageReader> reader{mock_reader};
    RegistrationSuccessfulMessage message{RegistrationSuccessfulMessage::parse_from_incoming_message(reader, 3)};

    buuid::string_generator gen;
    buuid::uuid uuid{gen("bb4f4460-3c25-11ed-b2bb-f2d20dd24480")};
    ASSERT_EQ(message.get_uuid(), uuid);
}

TEST(ServerMessageTest, wrong_version) {
    MockIncomingMessageReader* mock_reader = new MockIncomingMessageReader;
    EXPECT_CALL(*mock_reader, read_uint8())
        .WillOnce(Return(2));

    shared_ptr<MockIncomingMessageReader> reader{mock_reader};
    ASSERT_THROW(
        {
            RegistrationSuccessfulMessage message{RegistrationSuccessfulMessage::parse_from_incoming_message(reader, 3)};
        },
        client::protocol::FailedToParseMessage);
}

TEST(ServerMessageTest, wrong_code) {
    MockIncomingMessageReader* mock_reader = new MockIncomingMessageReader;
    EXPECT_CALL(*mock_reader, read_uint8())
        .WillOnce(Return(3));
    EXPECT_CALL(*mock_reader, read_uint16())
        .WillOnce(Return(2103));

    shared_ptr<MockIncomingMessageReader> reader{mock_reader};
    ASSERT_THROW(
        {
            RegistrationSuccessfulMessage message{RegistrationSuccessfulMessage::parse_from_incoming_message(reader, 3)};
        },
        client::protocol::FailedToParseMessage);
}

TEST(ServerMessageTest, AES_key_message) {
    string aes_key{"01234567890123456"};
    MockIncomingMessageReader* mock_reader = new MockIncomingMessageReader;
    EXPECT_CALL(*mock_reader, read_uint8())
        .WillOnce(Return(3));
    EXPECT_CALL(*mock_reader, read_uint16())
        .WillOnce(Return(2102));
    EXPECT_CALL(*mock_reader, read_bytes(16))
        .WillOnce(Return(string{'\xbb', '\x4f', '\x44', '\x60', '\x3c', '\x25', '\x11', '\xed', '\xb2', '\xbb',
                                '\xf2', '\xd2', '\x0d', '\xd2', '\x44', '\x80'}))
        .WillOnce(Return(aes_key));  // We read 16 again since read_uint32 returns 16 + 16. Kind of confusing should probably change

    EXPECT_CALL(*mock_reader, read_uint32())
        .WillOnce(Return(16 + 16));

    shared_ptr<MockIncomingMessageReader> reader{mock_reader};
    AESKeyMessage message{AESKeyMessage::parse_from_incoming_message(reader, 3)};

    ASSERT_EQ(message.get_encrypted_aes_key(), aes_key);
}

TEST(ServerMessageTest, upload_file_successful_message) {
    client::util::NameString filename{"coolfileame"};
    uint32_t checksum{0x12345};
    MockIncomingMessageReader* mock_reader = new MockIncomingMessageReader;
    EXPECT_CALL(*mock_reader, read_uint8())
        .WillOnce(Return(3));
    EXPECT_CALL(*mock_reader, read_uint16())
        .WillOnce(Return(2103));
    EXPECT_CALL(*mock_reader, read_bytes(16))
        .WillOnce(Return(string{'\xbb', '\x4f', '\x44', '\x60', '\x3c', '\x25', '\x11', '\xed', '\xb2', '\xbb',
                                '\xf2', '\xd2', '\x0d', '\xd2', '\x44', '\x80'}));
    EXPECT_CALL(*mock_reader, read_bytes(255))
        .WillOnce(Return(string{filename.get_name().data()}));

    EXPECT_CALL(*mock_reader, read_uint32())
        .WillOnce(Return(16 + 4 + 255 + 4))  //  payload_size
        .WillOnce(Return(100))               // content_size
        .WillOnce(Return(checksum));

    shared_ptr<MockIncomingMessageReader> reader{mock_reader};
    UploadFileSuccessfulMessage message{UploadFileSuccessfulMessage::parse_from_incoming_message(reader, 3)};

    ASSERT_EQ(message.get_filename(), filename);
    ASSERT_EQ(message.get_checksum(), checksum);
}

TEST(ServerMessageTest, success_message) {
    MockIncomingMessageReader* mock_reader = new MockIncomingMessageReader;
    EXPECT_CALL(*mock_reader, read_uint8())
        .WillOnce(Return(3));
    EXPECT_CALL(*mock_reader, read_uint16())
        .WillOnce(Return(2104));
    EXPECT_CALL(*mock_reader, read_uint32())
        .WillOnce(Return(0));

    shared_ptr<MockIncomingMessageReader> reader{mock_reader};
    SuccessResponseMessage message{SuccessResponseMessage::parse_from_incoming_message(reader, 3)};
}
