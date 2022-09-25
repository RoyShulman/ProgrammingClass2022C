#pragma once
#include <boost/uuid/uuid.hpp>
#include <cstdint>
#include <stdexcept>
#include <string>

#include "../util/name_string.h"

namespace buuid = boost::uuids;
using std::invalid_argument;
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

class InvalidMessageArgument : public invalid_argument {
public:
    explicit InvalidMessageArgument(const string& error);
};

class RegistrationRequestMessage : public ClientMessage {
public:
    RegistrationRequestMessage(ClientVersion version, ClientMessageID code, buuid::uuid uuid, util::NameString name);

    virtual string pack() const override;

private:
    util::NameString name_;
};

}  // namespace protocol
}  // namespace client
