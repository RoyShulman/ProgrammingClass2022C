#include "private_key_wrapper.h"

#include <cryptopp/base64.h>
#include <cryptopp/osrng.h>
#include <cryptopp/queue.h>

namespace client {
namespace encryption {

FailedToLoadPrivateKeyFromBase64::FailedToLoadPrivateKeyFromBase64(const string& base64_key, const string& error)
    : invalid_argument("Failed to load private key: " + base64_key + "\nError: " + error) {}

FailedToGenerateKeyWithSize::FailedToGenerateKeyWithSize(size_t bits)
    : invalid_argument("Failed to generate private key with size: " + std::to_string(bits)) {}

PrivateKeyWrapper::PrivateKeyWrapper(size_t bits) {
    CryptoPP::AutoSeededRandomPool rng;
    try {
        key_.GenerateRandomWithKeySize(rng, bits);
    } catch (const std::exception& e) {
        throw FailedToGenerateKeyWithSize{bits};
    }
}

PrivateKeyWrapper::PrivateKeyWrapper(const string& base64_key) {
    // Based on https://cryptopp.com/wiki/Keys_and_Formats and https://www.cryptopp.com/wiki/Pipelining
    CryptoPP::ByteQueue queue;
    try {
        // According to the docs - StringSource now owns Base64Decoder and Redirector, and will free it when it is deleted
        CryptoPP::StringSource source(base64_key, true,
                                      new CryptoPP::Base64Decoder(
                                          new CryptoPP::Redirector(queue)));
        queue.MessageEnd();
        key_.BERDecodePrivateKey(queue, false, queue.MaxRetrievable());
    } catch (const std::exception& e) {
        throw FailedToLoadPrivateKeyFromBase64{base64_key, e.what()};
    }

    if (!queue.IsEmpty()) {
        throw FailedToLoadPrivateKeyFromBase64(base64_key, "Queue should be empty");
    }
}

string PrivateKeyWrapper::base64() const {
    string base64_string;
    // According to the docs, Base64Encoder now owns StringSink and will free it when it is deleted
    CryptoPP::Base64Encoder base64_encoder(new CryptoPP::StringSink(base64_string), false);
    key_.DEREncodePrivateKey(base64_encoder);
    base64_encoder.MessageEnd();

    return base64_string;
}

bool PrivateKeyWrapper::operator==(const PrivateKeyWrapper& rhs) const {
    return base64() == rhs.base64();
}
bool PrivateKeyWrapper::operator!=(const PrivateKeyWrapper& rhs) const {
    return !operator==(rhs);
}
std::ostream& operator<<(std::ostream& stream, const PrivateKeyWrapper& key) {
    return stream << key.base64();
}

}  // namespace encryption
}  // namespace client
