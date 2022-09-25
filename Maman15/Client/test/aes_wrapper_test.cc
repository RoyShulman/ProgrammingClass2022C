#include "Maman15/Client/encryption/aes_wrapper.h"

#include <gtest/gtest.h>

TEST(AESWrapperTest, wrong_key_size) {
    ASSERT_THROW(
        {
            client::encryption::AESWrapper aes{"1234"};
        },
        client::encryption::InvalidKeyException);
}

TEST(AESWrapperTest, encryption) {
    client::encryption::AESWrapper aes{"0123467890123456"};
    string plain{'h', 'e', 'l', 'l', 'o', ' ', 't', 'h', 'e', 'r', 'e'};
    string expected{'E', ':', 'i', '\xcc', '\'', '\xb3', '\xa9', '\xd3', '\x14', '5', '\xad', '\xfe', '\xd5', '(', 'v', '\xab'};
    ASSERT_EQ(expected, aes.encrypt(plain));
}
