#pragma once

#include <cryptopp/aes.h>
#include <cryptopp/modes.h>

#include <array>
#include <stdexcept>
#include <string>

using std::array;
using std::invalid_argument;
using std::string;

namespace client {
namespace encryption {

class InvalidKeyException : public invalid_argument {
public:
    explicit InvalidKeyException(const string& error);
};

class AESWrapper {
public:
    explicit AESWrapper(const string& key);

    /**
     * @brief Encrypt the plain using the aes key
     * 
     * @param plain - String to encrypt
     * @return The encrypted string
     */
    string encrypt(const string& plain);

private:
    array<CryptoPP::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> key_;
};

}  // namespace encryption
}  // namespace client
