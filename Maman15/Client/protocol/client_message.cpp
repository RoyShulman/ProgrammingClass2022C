#include "client_message.h"

#include <utility>

#include "../util/little_endian_string_stream.h"

namespace client {
namespace protocol {

ClientMessage::ClientMessage(ClientVersion version, ClientMessageID code, buuid::uuid uuid)
    : version_(version), code_(code), uuid_(std::move(uuid)) {}

string ClientMessage::pack_header() const {
    string header;
    util::LittleEndianStringStream packing_stream{header};

    packing_stream << uuid_ << version_ << static_cast<uint16_t>(code_) << get_payload_size();
    return header;
}

RegistrationRequestMessage::RegistrationRequestMessage(ClientVersion version, buuid::uuid uuid, util::NameString name)
    : ClientMessage(version, ClientMessageID::REGISTRATION, std::move(uuid)), name_(std::move(name)) {}

string RegistrationRequestMessage::pack() const {
    string message;
    util::LittleEndianStringStream packing_stream{message};
    packing_stream << pack_header() << name_;
    return message;
}

uint32_t RegistrationRequestMessage::get_payload_size() const {
    return name_.get_name().size();
}

ClientPublicKeyMessage::ClientPublicKeyMessage(ClientVersion version, buuid::uuid uuid, util::NameString name, string public_key)
    : ClientMessage(version, ClientMessageID::CLIENT_PUBLIC_KEY, std::move(uuid)),
      name_(std::move(name)),
      public_key_(std::move(public_key)) {}

string ClientPublicKeyMessage::pack() const {
    string message;
    util::LittleEndianStringStream packing_stream{message};
    packing_stream << pack_header() << name_ << public_key_;
    return message;
}

uint32_t ClientPublicKeyMessage::get_payload_size() const {
    return name_.get_name().size() + public_key_.size();
}

UploadFileMessage::UploadFileMessage(ClientVersion version, buuid::uuid uuid, util::NameString file_name, string encrypted_file_content)
    : ClientMessage(version, ClientMessageID::UPLOAD_FILE, std::move(uuid)),
      file_name_(std::move(file_name)),
      encrypted_file_content_(std::move(encrypted_file_content)) {}

string UploadFileMessage::pack() const {
    string message;
    util::LittleEndianStringStream packing_stream{message};
    packing_stream << pack_header() << static_cast<uint32_t>(encrypted_file_content_.size()) << file_name_ << encrypted_file_content_;
    return message;
}

uint32_t UploadFileMessage::get_payload_size() const {
    return file_name_.get_name().size() + sizeof(uint32_t) + encrypted_file_content_.size();
}

EmptyPayloadClientMessage::EmptyPayloadClientMessage(ClientVersion version, ClientMessageID code, buuid::uuid uuid)
    : ClientMessage(version, code, std::move(uuid)) {}

string EmptyPayloadClientMessage::pack() const {
    string message;
    util::LittleEndianStringStream packing_stream{message};
    packing_stream << pack_header();
    return message;
}

uint32_t EmptyPayloadClientMessage::get_payload_size() const {
    return 0;
}

FileCRCOkMessage::FileCRCOkMessage(ClientVersion version, buuid::uuid uuid)
    : EmptyPayloadClientMessage(version, ClientMessageID::FILE_CRC_OK, std::move(uuid)) {}

CRCIncorrectWillRetry::CRCIncorrectWillRetry(ClientVersion version, buuid::uuid uuid)
    : EmptyPayloadClientMessage(version, ClientMessageID::CRC_INCORRECT_WILL_RETRY, std::move(uuid)) {}

CRCIncorrectGivingUp::CRCIncorrectGivingUp(ClientVersion version, buuid::uuid uuid)
    : EmptyPayloadClientMessage(version, ClientMessageID::CRC_INCORRECT_GIVING_UP, std::move(uuid)) {}

}  // namespace protocol
}  // namespace client
