#include "Maman14/Server/request_reader.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "Maman14/Server/connection_manager.h"

using std::unique_ptr;
using ::testing::Return;

class MockConnectionMangager : public AbstractConnectionManager {
public:
    MOCK_METHOD1(send, void(const vector<uint8_t>&));
    MOCK_METHOD1(recv, vector<uint8_t>(size_t));
};

TEST(RquestReaderTest, read_uint8) {
    unique_ptr<MockConnectionMangager> mock_connection(new MockConnectionMangager());
    vector<uint8_t> vec{2};
    EXPECT_CALL(*mock_connection, recv(1))
        .WillOnce(Return(vec));

    RequestReader reader(std::move(mock_connection));
    ASSERT_EQ(2, reader.read_uint8());
}

TEST(RquestReaderTest, read_uint16) {
    unique_ptr<MockConnectionMangager> mock_connection(new MockConnectionMangager());
    vector<uint8_t> vec{0x2, 0x25};
    EXPECT_CALL(*mock_connection, recv(2))
        .WillOnce(Return(vec));

    RequestReader reader(std::move(mock_connection));
    ASSERT_EQ(9474, reader.read_uint16());
}

TEST(RquestReaderTest, read_uint32) {
    unique_ptr<MockConnectionMangager> mock_connection(new MockConnectionMangager());
    vector<uint8_t> vec{0x2, 0x25, 0x12, 0xfb};
    EXPECT_CALL(*mock_connection, recv(4))
        .WillOnce(Return(vec));

    RequestReader reader(std::move(mock_connection));
    ASSERT_EQ(4212270338, reader.read_uint32());
}

TEST(RquestReaderTest, read_bytes) {
    unique_ptr<MockConnectionMangager> mock_connection(new MockConnectionMangager());
    vector<uint8_t> vec{'a', 'c', '1', 'g', '\x12', 255};
    EXPECT_CALL(*mock_connection, recv(vec.size()))
        .WillOnce(Return(vec));

    RequestReader reader(std::move(mock_connection));
    ASSERT_EQ(vec, reader.read_bytes(vec.size()));
}
