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

InvalidMessageArgument::InvalidMessageArgument(const string& error)
    : invalid_argument("InvalidMessageArgument: " + error) {}

RegistrationRequestMessage::RegistrationRequestMessage(ClientVersion version, ClientMessageID code,
                                                       buuid::uuid uuid, util::NameString name)
    : ClientMessage(version, code, name.get_name().size(), uuid), name_(std::move(name)) {}

string RegistrationRequestMessage::pack() const {
    string message;
    util::LittleEndianStringStream packing_stream{message};
    packing_stream << pack_header() << name_;
    return message;
}

}  // namespace protocol
}  // namespace client
