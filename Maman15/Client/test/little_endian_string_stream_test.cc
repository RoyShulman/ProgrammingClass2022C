#include "Maman15/Client/util/little_endian_string_stream.h"

#include <gtest/gtest.h>

#include <string>

using std::string;

TEST(LittleEndianStringStream, uint8_test) {
    string s;
    client::util::LittleEndianStringStream l(s);
    uint8_t value{1};
    l << value;
    string expected{'\x01'};
    ASSERT_EQ(s, expected);
}

TEST(LittleEndianStringStream, uint16_test) {
    string s;
    client::util::LittleEndianStringStream l(s);
    uint16_t value{1};
    l << value;
    string expected{'\x01', '\x00'};
    ASSERT_EQ(s, expected);
}

TEST(LittleEndianStringStream, uint32_test) {
    string s;
    client::util::LittleEndianStringStream l(s);
    uint32_t value{0xf123bfd};
    l << value;
    string expected{"\xfd;\x12\x0f"};
    ASSERT_EQ(s, expected);
}

TEST(LittleEndianStringStream, string_test) {
    string s{"abc"};
    client::util::LittleEndianStringStream l(s);
    string value{"\xfd;\x12\x0f"};
    l << value;
    string expected{"abc\xfd;\x12\x0f"};
    ASSERT_EQ(s, expected);
}
