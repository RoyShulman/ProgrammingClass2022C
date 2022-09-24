#include "Maman15/Client/user_info.h"

#include <gtest/gtest.h>

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid.hpp>

#include "Maman15/Client/encryption/private_key_wrapper.h"

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

TEST(user_info_test, valid_file) {
    bfs::path path{"Maman15/Client/test/resources/me.info"};
    boost::uuids::string_generator gen;
    boost::uuids::uuid uuid{gen("64f3f63985f04beb81a0e43321880182")};

    client::UserInfo info{client::UserInfo::from_file(path)};
    client::UserInfo expected{string{"Michael Jackson"},
                              uuid,
                              client::encryption::PrivateKeyWrapper(private_key)};
    ASSERT_EQ(info, expected);
}

class InvalidUserFileTest : public testing::TestWithParam<const char*> {};

TEST_P(InvalidUserFileTest, invalid_file) {
    bfs::path path{"Maman15/Client/test/resources"};
    path /= GetParam();
    ASSERT_THROW(
        {
            client::UserInfo info{client::UserInfo::from_file(path)};
        },
        client::InvalidUserInfoFile);
}

INSTANTIATE_TEST_SUITE_P(InvalidInfoFilePaths,
                         InvalidUserFileTest,
                         testing::Values("too_short_me.info", "too_long_me.info", "invalid_key_me.info"));
