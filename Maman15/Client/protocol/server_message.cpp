#include "server_message.h"

#include <boost/uuid/string_generator.hpp>

using std::exception;

namespace client {
namespace protocol {

WrongMessageVersion::WrongMessageVersion(ServerVersion expected, ServerVersion read)
    : runtime_error("Expected message version: " + std::to_string(expected) + ", got " + std::to_string(read)) {}

WrongMessageCode::WrongMessageCode(ServerMessageID expected, ServerMessageID read)
    : runtime_error("Expected message code: " + std::to_string(static_cast<uint16_t>(expected)) + ", got " + std::to_string(static_cast<uint16_t>(read))),
      read_(read) {}

FailedToParseMessage::FailedToParseMessage(const string& message, const string& error)
    : runtime_error("Failed to parse message: " + message + " With error: " + error) {}

ServerMessage::ServerMessage(ServerVersion version, ServerMessageID code, buuid::uuid uuid, uint32_t payload_size)
    : version_{version}, code_{code}, uuid_{std::move(uuid)}, payload_size_{payload_size} {}

RegistrationFailedException::RegistrationFailedException() {}

ServerMessage ServerMessage::parse_header_from_incoming_message(shared_ptr<AbstractIncomingMessageReader> message,
                                                                ServerMessageID expected_code, ServerVersion expected_version) {
    if (!message) {
        throw std::invalid_argument("message cannot be null");
    }

    uint8_t version{message->read_uint8()};
    if (version != expected_version) {
        throw WrongMessageVersion{expected_version, version};
    }

    ServerMessageID code{static_cast<ServerMessageID>(message->read_uint16())};
    if (code != expected_code) {
        throw WrongMessageCode{expected_code, code};
    }
    uint32_t payload_size{message->read_uint32()};

    buuid::uuid uuid;
    memcpy(uuid.data, message->read_bytes(buuid::uuid::static_size()).data(), buuid::uuid::static_size());

    return {version, code, uuid, payload_size};
}

RegistrationSuccessfulMessage::RegistrationSuccessfulMessage(ServerVersion version, buuid::uuid uuid, uint32_t payload_size)
    : ServerMessage(version, ServerMessageID::REGISTRATION_SUCCESSFUL, std::move(uuid), payload_size) {}

RegistrationSuccessfulMessage RegistrationSuccessfulMessage::parse_from_incoming_message(shared_ptr<AbstractIncomingMessageReader> message,
                                                                                         ServerVersion expected_version) {
    try {
        if (!message) {
            throw std::invalid_argument("message cannot be null");
        }
        ServerMessage header{parse_header_from_incoming_message(message,
                                                                ServerMessageID::REGISTRATION_SUCCESSFUL,
                                                                expected_version)};
        return {header.get_version(), header.get_uuid(), header.get_payload_size()};
    } catch (const WrongMessageCode& e) {
        if (e.get_read() == ServerMessageID::REGISTRATION_FAILED) {
            throw RegistrationFailedException();
        }
        throw FailedToParseMessage("RegistrationSuccessfulMessage failed with unexpected message code: ", e.what());
    } catch (const exception& e) {
        throw FailedToParseMessage("RegistrationSuccessfulMessage", e.what());
    }
}

AESKeyMessage::AESKeyMessage(ServerVersion version, buuid::uuid uuid, uint32_t payload_size, string encrypted_aes_key)
    : ServerMessage(version, ServerMessageID::AES_KEY, std::move(uuid), payload_size), encrypted_aes_key_(std::move(encrypted_aes_key)) {}

AESKeyMessage AESKeyMessage::parse_from_incoming_message(shared_ptr<AbstractIncomingMessageReader> message,
                                                         ServerVersion expected_version) {
    try {
        if (!message) {
            throw std::invalid_argument("message cannot be null");
        }
        ServerMessage header{parse_header_from_incoming_message(message,
                                                                ServerMessageID::AES_KEY,
                                                                expected_version)};
        string encrypted_aes_key{message->read_bytes(header.get_payload_size() - buuid::uuid::static_size())};

        return {header.get_version(), header.get_uuid(), header.get_payload_size(), encrypted_aes_key};
    } catch (const exception& e) {
        throw FailedToParseMessage("AESKeyMessage", e.what());
    }
}

UploadFileSuccessfulMessage::UploadFileSuccessfulMessage(ServerVersion version, buuid::uuid uuid, uint32_t payload_size,
                                                         uint32_t content_size, util::NameString filename, uint32_t checksum)
    : ServerMessage(version, ServerMessageID::UPLOAD_FILE_SUCCESSFUL, std::move(uuid), payload_size),
      content_size_{content_size},
      filename_(std::move(filename)),
      checksum_(checksum) {}

UploadFileSuccessfulMessage UploadFileSuccessfulMessage::parse_from_incoming_message(shared_ptr<AbstractIncomingMessageReader> message,
                                                                                     ServerVersion expected_version) {
    try {
        if (!message) {
            throw std::invalid_argument("message cannot be null");
        }
        ServerMessage header{parse_header_from_incoming_message(message,
                                                                ServerMessageID::UPLOAD_FILE_SUCCESSFUL,
                                                                expected_version)};
        uint32_t content_size{message->read_uint32()};
        util::NameString filename{message->read_bytes(util::NameString::get_name_size())};
        uint32_t checksum{message->read_uint32()};
        return {header.get_version(), header.get_uuid(), header.get_payload_size(), content_size, filename, checksum};
    } catch (const exception& e) {
        throw FailedToParseMessage("UploadFileSuccessful", e.what());
    }
}

SuccessResponseMessage::SuccessResponseMessage(ServerVersion version, buuid::uuid uuid, uint32_t payload_size)
    : ServerMessage(version, ServerMessageID::SUCCESS_RESPONSE, std::move(uuid), payload_size) {}

SuccessResponseMessage SuccessResponseMessage::parse_from_incoming_message(shared_ptr<AbstractIncomingMessageReader> message,
                                                                           ServerVersion expected_version) {
    try {
        if (!message) {
            throw std::invalid_argument("message cannot be null");
        }
        ServerMessage header{parse_header_from_incoming_message(message,
                                                                ServerMessageID::SUCCESS_RESPONSE,
                                                                expected_version)};
        return {header.get_version(), header.get_uuid(), header.get_payload_size()};
    } catch (const exception& e) {
        throw FailedToParseMessage("SuccessResponseMessage", e.what());
    }
}

}  // namespace protocol
}  // namespace client
