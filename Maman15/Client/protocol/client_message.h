#pragma once
#include <boost/uuid/uuid.hpp>
#include <cstdint>
#include <string>

#include "../util/name_string.h"

namespace buuid = boost::uuids;
using std::string;

namespace client {
namespace protocol {

typedef uint8_t ClientVersion;

enum class ClientMessageID : uint16_t {
    REGISTRATION = 1100,
    CLIENT_PUBLIC_KEY = 1101,
    UPLOAD_FILE = 1103,
    FILE_CRC_OK = 1104,
    CRC_INCORRECT_WILL_RETRY = 1105,
    CRC_INCORRECT_GIVING_UP = 1106,
};

/**
 * @brief Abstract base class for all user messages.
 */
class ClientMessage {
protected:
    ClientMessage(ClientVersion version, ClientMessageID code, uint32_t payload_size, buuid::uuid uuid);
    virtual ~ClientMessage() = default;

    string pack_header() const;

    virtual string pack() const = 0;

private:
    ClientVersion version_;
    ClientMessageID code_;
    uint32_t payload_size_;
    buuid::uuid uuid_;
};

class RegistrationRequestMessage : public ClientMessage {
public:
    RegistrationRequestMessage(ClientVersion version, buuid::uuid uuid, util::NameString name);

    virtual string pack() const override;

private:
    util::NameString name_;
};

class ClientPublicKeyMessage : public ClientMessage {
public:
    ClientPublicKeyMessage(ClientVersion version, buuid::uuid uuid, util::NameString name, string public_key);

    virtual string pack() const override;

private:
    util::NameString name_;
    string public_key_;
};

}  // namespace protocol
}  // namespace client
