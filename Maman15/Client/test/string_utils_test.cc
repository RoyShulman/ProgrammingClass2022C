#include "Maman15/Client/util/string_utils.h"

#include <gtest/gtest.h>

TEST(test_string_to_endpoint, good_address) {
    auto real = client::util::string_to_endpoint("127.0.0.1:1337");
    bip::tcp::endpoint expected{bip::address::from_string("127.0.0.1"), 1337};
    ASSERT_EQ(real, expected);
}

TEST(test_string_to_endpoint, invalid_address) {
    ASSERT_THROW(
        {
            client::util::string_to_endpoint("127.0.0.11337");
        },
        client::util::StringIsNotAValidEndpoint);
}
