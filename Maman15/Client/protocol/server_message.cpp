#include "server_message.h"

namespace client {
namespace protocol {

ServerMessage::ServerMessage(ServerVersion version, ServerMessageID code, buuid::uuid uuid)
    : version_(version), code_(code), uuid_(std::move(uuid)) {}

RegistrationSuccessfulMessage::RegistrationSuccessfulMessage(ServerVersion version, buuid::uuid uuid)
    : ServerMessage(version, ServerMessageID::REGISTRATION_SUCCESSFUL, std::move(uuid)) {}

AESKeyMessage::AESKeyMessage(ServerVersion version, buuid::uuid uuid, string aes_key)
    : ServerMessage(version, ServerMessageID::AES_KEY, std::move(uuid)), aes_key_(std::move(aes_key)) {}

UploadFileSuccessful::UploadFileSuccessful(ServerVersion version, buuid::uuid uuid, util::NameString filename, uint32_t checksum)
    : ServerMessage(version, ServerMessageID::UPLOAD_FILE_SUCCESSFUL, std::move(uuid)), filename_(std::move(filename)), checksum_(checksum) {}
}  // namespace protocol
}  // namespace client
