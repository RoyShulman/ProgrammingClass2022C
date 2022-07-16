#include "Maman14/Server/bytearray.h"

#include <gtest/gtest.h>

#include <string>

using std::string;
using utils::Bytearray;

TEST(bytearray_tests, basic_test) {
    Bytearray buffer;
    uint8_t expected[2] = {1, 5};

    buffer.push_u8(expected[0]);
    buffer.push_u8(expected[1]);

    ASSERT_EQ(2, buffer.len());

    const uint8_t* actual = buffer.data();
    for (size_t i = 0; i < buffer.len(); i++) {
        ASSERT_EQ(expected[i], actual[i]);
    }
}

TEST(bytearray_tests, uint16_test) {
    Bytearray buffer;
    uint16_t expected[2] = {5, 16};

    buffer.push_u16(expected[0]);
    buffer.push_u16(expected[1]);

    ASSERT_EQ(4, buffer.len());
    ASSERT_TRUE(memcmp(expected, buffer.data(), buffer.len()) == 0);
}

TEST(bytearray_tests, uint32_test) {
    Bytearray buffer;
    uint32_t expected[2] = {5, 16};

    buffer.push_u32(expected[0]);
    buffer.push_u32(expected[1]);

    ASSERT_EQ(8, buffer.len());
    ASSERT_TRUE(memcmp(expected, buffer.data(), buffer.len()) == 0);
}

TEST(bytearray_tests, str_test) {
    Bytearray buffer;
    string expected("hello");

    buffer.push_string(expected);

    ASSERT_EQ(5, buffer.len());
    ASSERT_TRUE(memcmp(expected.c_str(), buffer.data(), buffer.len()) == 0);
}

TEST(bytearray_tests, bytearray_concat_test) {
    Bytearray buffer1;
    Bytearray buffer2;

    uint8_t expected[4] = {1, 6, 2, 7};

    buffer1.push_u8(expected[0]);
    buffer1.push_u8(expected[1]);
    buffer2.push_u8(expected[2]);
    buffer2.push_u8(expected[3]);

    buffer1.push_bytes(buffer2);

    ASSERT_EQ(4, buffer1.len());
    ASSERT_TRUE(memcmp(expected, buffer1.data(), buffer1.len()) == 0);
}
