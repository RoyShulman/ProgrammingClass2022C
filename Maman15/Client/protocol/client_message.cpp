#include "client_message.h"

#include <utility>

#include "../util/little_endian_string_stream.h"

namespace client {
namespace protocol {

ClientMessage::ClientMessage(ClientVersion version, ClientMessageID code, uint32_t payload_size, buuid::uuid uuid)
    : version_(version), code_(code), payload_size_(payload_size), uuid_(std::move(uuid)) {}

string ClientMessage::pack_header() const {
    string header;
    util::LittleEndianStringStream packing_stream{header};

    packing_stream << uuid_ << version_ << static_cast<uint16_t>(code_) << payload_size_;
    return header;
}

RegistrationRequestMessage::RegistrationRequestMessage(ClientVersion version, buuid::uuid uuid, util::NameString name)
    : ClientMessage(version, ClientMessageID::REGISTRATION, name.get_name().size(), std::move(uuid)), name_(std::move(name)) {}

string RegistrationRequestMessage::pack() const {
    string message;
    util::LittleEndianStringStream packing_stream{message};
    packing_stream << pack_header() << name_;
    return message;
}

ClientPublicKeyMessage::ClientPublicKeyMessage(ClientVersion version, buuid::uuid uuid, util::NameString name, string public_key)
    : ClientMessage(version, ClientMessageID::CLIENT_PUBLIC_KEY, name.get_name().size() + public_key.size(), std::move(uuid)),
      name_(std::move(name)),
      public_key_(std::move(public_key)) {}

string ClientPublicKeyMessage::pack() const {
    string message;
    util::LittleEndianStringStream packing_stream{message};
    packing_stream << pack_header() << name_ << public_key_;
    return message;
}

}  // namespace protocol
}  // namespace client
