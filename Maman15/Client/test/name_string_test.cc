#include "Maman15/Client/util/name_string.h"

#include <gtest/gtest.h>

TEST(NameStringTest, good_length) {
    string s{"hello there"};
    char expected[255] = {0};
    strcpy(expected, "hello there");
    client::util::NameString name{s};
    ASSERT_TRUE(memcmp(expected, name.get_name().data(), 255) == 0);
}

class InvalidLengthNameStringTest : public testing::TestWithParam<unsigned long> {};

TEST_P(InvalidLengthNameStringTest, bad_length) {
    string s(GetParam(), '*');
    ASSERT_THROW(
        {
            client::util::NameString name{s};
        },
        client::util::StringLengthTooLarge);
}
INSTANTIATE_TEST_SUITE_P(InvalidInfoFileLengths,
                         InvalidLengthNameStringTest,
                         testing::Values(255, 300, 1000));
