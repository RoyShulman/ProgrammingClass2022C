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
                                                       buuid::uuid uuid, string name)
    : ClientMessage(version, code, MAX_NAME_SIZE, uuid), name_(std::move(name)) {
    if (name_.size() > MAX_NAME_SIZE) {
        throw InvalidMessageArgument("length of: " + name_ + " must be smaller than: " + std::to_string(MAX_NAME_SIZE));
    }
}

string RegistrationRequestMessage::pack() const {
    string message;
    util::LittleEndianStringStream packing_stream{message};
    packing_stream << pack_header() << name_;
    return message;
}

}  // namespace protocol
}  // namespace client
