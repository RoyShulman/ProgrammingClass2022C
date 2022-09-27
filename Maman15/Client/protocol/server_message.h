#pragma once
#include <boost/uuid/uuid.hpp>
#include <cstdint>
#include <string>

#include "../util/name_string.h"

namespace buuid = boost::uuids;
using std::string;

namespace client {
namespace protocol {

typedef uint8_t ServerVersion;

enum class ServerMessageID : uint16_t {
    REGISTRATION_SUCCESSFUL = 2100,
    AES_KEY = 2102,
    UPLOAD_FILE_SUCCESSFUL = 2103,
    SUCCESS_RESPONSE = 2104,
};

/**
 * @brief Abstract base class for all server messages.
 */
class ServerMessage {
protected:
    ServerMessage(ServerVersion version, ServerMessageID code, buuid::uuid uuid);
    virtual ~ServerMessage() = default;

    virtual uint32_t get_payload_size() const = 0;

private:
    ServerVersion version_;
    ServerMessageID code_;
    buuid::uuid uuid_;
};

class RegistrationSuccessfulMessage : public ServerMessage {
public:
    RegistrationSuccessfulMessage(ServerVersion version, buuid::uuid uuid);

    uint32_t get_payload_size() const override { return EXPECTED_PAYLOAD_SIZE_; };

private:
    static constexpr size_t EXPECTED_PAYLOAD_SIZE_ = 16;
};

class AESKeyMessage : public ServerMessage {
public:
    AESKeyMessage(ServerVersion version, buuid::uuid uuid, string aes_key);

    uint32_t get_payload_size() const override { return EXPECTED_PAYLOAD_SIZE_; };

private:
    static constexpr size_t EXPECTED_PAYLOAD_SIZE_ = 32;
    string aes_key_;
};

class AESKeyMessage : public ServerMessage {
public:
    AESKeyMessage(ServerVersion version, buuid::uuid uuid, string aes_key);

    uint32_t get_payload_size() const override { return EXPECTED_PAYLOAD_SIZE_; };

private:
    static constexpr size_t EXPECTED_PAYLOAD_SIZE_ = 32;
    string aes_key_;
};

class UploadFileSuccessful : public ServerMessage {
public:
    UploadFileSuccessful(ServerVersion version, buuid::uuid uuid, util::NameString filename, uint32_t checksum);

    uint32_t get_payload_size() const override { return EXPECTED_PAYLOAD_SIZE_; };

private:
    static constexpr size_t EXPECTED_PAYLOAD_SIZE_ = 16 + 4 + 255 + 4;
    util::NameString filename_;
    uint32_t checksum_;
};

}  // namespace protocol
}  // namespace client
