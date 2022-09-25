#include "Maman15/Client/util/little_endian_string_stream.h"

#include <gtest/gtest.h>

#include <boost/uuid/string_generator.hpp>
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

TEST(LittleEndianStringStream, uuid_test) {
    string s;
    boost::uuids::string_generator gen;
    client::util::LittleEndianStringStream l(s);
    l << gen("bb4f4460-3c25-11ed-b2bb-f2d20dd24480");
    string expected{"\xbbOD`<%\x11\xed\xb2\xbb\xf2\xd2\r\xd2\x44\x80"};
    ASSERT_EQ(s, expected);
}

TEST(LittleEndianStringStream, name_string_test) {
    string s;
    client::util::LittleEndianStringStream l(s);

    string expected(255, '\0');
    expected[0] = 'a';
    expected[1] = '2';

    client::util::NameString name{"a2"};
    l << name;
    ASSERT_EQ(s, expected);
}
