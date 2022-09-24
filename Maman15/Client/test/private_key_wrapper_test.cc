#include "Maman15/Client/encryption/private_key_wrapper.h"

#include <gtest/gtest.h>

static const string private_key{
    "MIICWwIBAAKBgQC1DNt6d49QPbT6IIGjbbLphllE6WmjOUFYd91RQkN4H7z/7XOnFD2iVRec\n"
    "/zP0AoCY1PbmSiCa97U1PgwVOOf4iquWsUf2Dxd7+7ib9cRy4i5OrTZGfc4LlFPa1qtcwF4l\n"
    "l/5AkL03QjvO/hAzqXQCihg57P8p/AuByxDXCCfZCwIBEQKBgAHGZos9aE6XMXXnOHyVJ2au\n"
    "fl2J0VlxgKQBLNYhRu+b60PDlTZYCqS6j0pS0NK0uvx6jvg38ULGMXZ7lrTp+DwlP9q8W7aZ\n"
    "VCenuLHbx7G8qrkY6F4UepThuMrI+mIE7pMLXvSWX/wC3aym3leOxgaueBUf0IDQdYinVJ0J\n"
    "pqH5AkEAueFJYO5B9xsOupVbqhutOPs3UJKuM1PzYnM07+2QpOCstnbQdA+AuzuBvtOt2O04\n"
    "A5ZLE5fozdKOTrQz53sGQwJBAPlZJkPO8wB0oG2MYroX9H0tV3AOHCGixkI/2dL6viRu4lmx\n"
    "/DJvTWKtgsKlou2gIQKphCxT7h9kgNCmEUhISZkCQDarrCuRXrIXBFT+wJtxjUz+l8xnYGlz\n"
    "C1kw8XO+V7gF2HHmtcfIUwnkU1Y+QjC+Paazu7p3+S16KdrptOm6xZsCQQC+rZW7YgUed02A\n"
    "8uIV1hVQqjPOKOhV9PH2bQ/7sK+FRbwmeQwmkVlLdaA6fqnExb7j6wq4fGrMtkRjUdD7Chop\n"
    "AkEAsHfoo2lsEwfefft7PjSmYVOh3OK+/qxJjaCP6JN6CqGbtDvrfEWq27376xCPCDL2w51G\n"
    "eoqWIZlThkILHmStXA==\n"};

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
