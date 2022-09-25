#pragma once

#include <cryptopp/rsa.h>

#include <stdexcept>
#include <string>

using std::invalid_argument;
using std::string;

namespace client {
namespace encryption {

class FailedToLoadPrivateKeyFromBase64 : public invalid_argument {
public:
    explicit FailedToLoadPrivateKeyFromBase64(const string& base64_key, const string& error);
};

class FailedToGenerateKeyWithSize : public invalid_argument {
public:
    explicit FailedToGenerateKeyWithSize(size_t bits);
};

class PrivateKeyWrapper {
public:
    /**
     * @brief Construct a new Private Key Wrapper object from the base64 encoded key
     */
    explicit PrivateKeyWrapper(const string& base64_key);

    /**
     * @brief Construct a new Private Key Wrapper object with an
     * CryptoPP::RSA::PrivateKey key with size of bits
     */
    explicit PrivateKeyWrapper(size_t bits);

    string base64() const;

    bool operator==(const PrivateKeyWrapper& rhs) const;
    bool operator!=(const PrivateKeyWrapper& rhs) const;
    friend std::ostream& operator<<(std::ostream& stream, const PrivateKeyWrapper& info);

    string get_public() const;

private:
    CryptoPP::RSA::PrivateKey private_key_;
    CryptoPP::RSA::PublicKey public_key_;
};

}  // namespace encryption
}  // namespace client
