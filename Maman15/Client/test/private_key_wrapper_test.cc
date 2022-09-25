#include "Maman15/Client/encryption/private_key_wrapper.h"

#include <cryptopp/rsa.h>
#include <gtest/gtest.h>

static const string private_key{
    "MIICWwIBAAKBgQC1DNt6d49QPbT6IIGjbbLphllE6WmjOUFYd91RQkN4H7z/7XOnFD2iVRec"
    "/zP0AoCY1PbmSiCa97U1PgwVOOf4iquWsUf2Dxd7+7ib9cRy4i5OrTZGfc4LlFPa1qtcwF4l"
    "l/5AkL03QjvO/hAzqXQCihg57P8p/AuByxDXCCfZCwIBEQKBgAHGZos9aE6XMXXnOHyVJ2au"
    "fl2J0VlxgKQBLNYhRu+b60PDlTZYCqS6j0pS0NK0uvx6jvg38ULGMXZ7lrTp+DwlP9q8W7aZ"
    "VCenuLHbx7G8qrkY6F4UepThuMrI+mIE7pMLXvSWX/wC3aym3leOxgaueBUf0IDQdYinVJ0J"
    "pqH5AkEAueFJYO5B9xsOupVbqhutOPs3UJKuM1PzYnM07+2QpOCstnbQdA+AuzuBvtOt2O04"
    "A5ZLE5fozdKOTrQz53sGQwJBAPlZJkPO8wB0oG2MYroX9H0tV3AOHCGixkI/2dL6viRu4lmx"
    "/DJvTWKtgsKlou2gIQKphCxT7h9kgNCmEUhISZkCQDarrCuRXrIXBFT+wJtxjUz+l8xnYGlz"
    "C1kw8XO+V7gF2HHmtcfIUwnkU1Y+QjC+Paazu7p3+S16KdrptOm6xZsCQQC+rZW7YgUed02A"
    "8uIV1hVQqjPOKOhV9PH2bQ/7sK+FRbwmeQwmkVlLdaA6fqnExb7j6wq4fGrMtkRjUdD7Chop"
    "AkEAsHfoo2lsEwfefft7PjSmYVOh3OK+/qxJjaCP6JN6CqGbtDvrfEWq27376xCPCDL2w51G"
    "eoqWIZlThkILHmStXA=="};

static const string public_key{
    '0', '\x81', '\x9d', '0', '\r', '\x06', '\t', '*', '\x86', 'H',
    '\x86', '\xf7', '\r', '\x01', '\x01', '\x01', '\x05', '\x00',
    '\x03', '\x81', '\x8b', '\x00', '0', '\x81', '\x87', '\x02',
    '\x81', '\x81', '\x00', '\xb5', '\x0c', '\xdb', 'z', 'w',
    '\x8f', 'P', '=', '\xb4', '\xfa', ' ', '\x81', '\xa3', 'm',
    '\xb2', '\xe9', '\x86', 'Y', 'D', '\xe9', 'i', '\xa3', '9',
    'A', 'X', 'w', '\xdd', 'Q', 'B', 'C', 'x', '\x1f', '\xbc',
    '\xff', '\xed', 's', '\xa7', '\x14', '=', '\xa2', 'U',
    '\x17', '\x9c', '\xff', '3', '\xf4', '\x02', '\x80', '\x98',
    '\xd4', '\xf6', '\xe6', 'J', ' ', '\x9a', '\xf7', '\xb5',
    '5', '>', '\x0c', '\x15', '8', '\xe7', '\xf8', '\x8a',
    '\xab', '\x96', '\xb1', 'G', '\xf6', '\x0f', '\x17',
    '{', '\xfb', '\xb8', '\x9b', '\xf5', '\xc4', 'r',
    '\xe2', '.', 'N', '\xad', '6', 'F', '}', '\xce',
    '\x0b', '\x94', 'S', '\xda', '\xd6', '\xab', '\\',
    '\xc0', '^', '%', '\x97', '\xfe', '@', '\x90',
    '\xbd', '7', 'B', ';', '\xce', '\xfe', '\x10', '3',
    '\xa9', 't', '\x02', '\x8a', '\x18', '9', '\xec',
    '\xff', ')', '\xfc', '\x0b', '\x81', '\xcb', '\x10',
    '\xd7', '\x08', '\'', '\xd9', '\x0b', '\x02', '\x01', '\x11'};

TEST(test_private_key_wrapper, test_base64) {
    client::encryption::PrivateKeyWrapper wrapper{private_key};
    ASSERT_EQ(private_key, wrapper.base64());
}

TEST(test_private_key_wrapper, test_equal) {
    client::encryption::PrivateKeyWrapper wrapper{private_key};
    client::encryption::PrivateKeyWrapper wrapper2{private_key};
    ASSERT_EQ(wrapper, wrapper2);
}

TEST(test_private_key_wrapper, test_not_equal) {
    client::encryption::PrivateKeyWrapper wrapper{private_key};
    client::encryption::PrivateKeyWrapper wrapper2{1024};
    ASSERT_NE(wrapper, wrapper2);
}

TEST(test_private_key_wrapper, test_bad_base64) {
    ASSERT_THROW(
        {
            client::encryption::PrivateKeyWrapper wrapper{"abc"};
        },
        client::encryption::FailedToLoadPrivateKeyFromBase64);
}

TEST(test_private_key_wrapper, test_stream_operator) {
    client::encryption::PrivateKeyWrapper wrapper{private_key};
    std::stringstream s;
    s << wrapper;
    ASSERT_EQ(private_key, s.str());
}

TEST(test_private_key_wrapper, test_get_public) {
    client::encryption::PrivateKeyWrapper wrapper{private_key};
    ASSERT_EQ(public_key, wrapper.get_public());
}
