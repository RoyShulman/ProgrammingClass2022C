#include "aes_wrapper.h"

#include <cryptopp/filters.h>

namespace client {
namespace encryption {

InvalidKeyException::InvalidKeyException(const string& error)
    : invalid_argument(error) {}

AESWrapper::AESWrapper(const string& key) {
    if (key.size() != key_.size()) {
        throw InvalidKeyException{"Given key: " + key + " is of wrong length"};
    }

    memcpy(key_.data(), key.data(), key_.size());
}
string AESWrapper::encrypt(const string& plain) {
    // Based on https://www.cryptopp.com/wiki/CBC_Mode
    string cipher;
    CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE] = {0};
    CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption aes_;
    aes_.SetKeyWithIV(key_.data(), key_.size(), iv, sizeof(iv));
    // By default StreamTransformationFilter uses pkcs7 padding, so in the python
    // code we must use AES.encrypt(Crypto.Util.Padding.pad(text, AES.block_size)) which also uses pkcs7 padding by default
    CryptoPP::StringSource ss(plain, true,
                              new CryptoPP::StreamTransformationFilter(aes_,
                                                                       new CryptoPP::StringSink(cipher)));
    return cipher;
}

}  // namespace encryption
}  // namespace client
