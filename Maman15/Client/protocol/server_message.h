#pragma once
#include <boost/uuid/uuid.hpp>
#include <cstdint>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>

#include "../incoming_message_reader.h"
#include "../util/name_string.h"

namespace buuid = boost::uuids;
using std::exception;
using std::runtime_error;
using std::shared_ptr;
using std::string;

namespace client {
namespace protocol {

typedef uint8_t ServerVersion;

enum class ServerMessageID : uint16_t {
    REGISTRATION_SUCCESSFUL = 2100,
    REGISTRATION_FAILED = 2101,
    AES_KEY = 2102,
    UPLOAD_FILE_SUCCESSFUL = 2103,
    SUCCESS_RESPONSE = 2104,
};

class WrongMessageVersion : public runtime_error {
public:
    WrongMessageVersion(ServerVersion expected, ServerVersion read);
};

class WrongMessageCode : public runtime_error {
public:
    WrongMessageCode(ServerMessageID expected, ServerMessageID read);

    ServerMessageID get_read() const { return read_; };

private:
    ServerMessageID read_;
};

class FailedToParseMessage : public runtime_error {
public:
    FailedToParseMessage(const string& message, const string& error);
};

class RegistrationFailedException : public exception {
public:
    RegistrationFailedException();
};

/**
 * @brief Abstract base class for all server messages.
 */
class ServerMessage {
protected:
    ServerMessage(ServerVersion version, ServerMessageID code, uint32_t payload_size);
    static ServerMessage parse_header_from_incoming_message(shared_ptr<AbstractIncomingMessageReader> message,
                                                            ServerMessageID expected_code, ServerVersion expected_version);

public:
    virtual ~ServerMessage() = default;

    ServerVersion get_version() const { return version_; };
    ServerMessageID get_code() const { return code_; };
    uint32_t get_payload_size() const { return payload_size_; };

private:
    ServerVersion version_;
    ServerMessageID code_;
    uint32_t payload_size_;
};

// All the following messages can parse from an incoming message.
// They should accept a pointer to an AbstractIncomingMessageReader to allow for polymorphism,
// parse_from_incoming_message isn't virtual since we want to return the concrete type of the message
class RegistrationSuccessfulMessage : public ServerMessage {
public:
    RegistrationSuccessfulMessage(ServerVersion version, uint32_t payload_size, buuid::uuid uuid);

    static RegistrationSuccessfulMessage parse_from_incoming_message(shared_ptr<AbstractIncomingMessageReader> message,
                                                                     ServerVersion expected_version);

    const buuid::uuid& get_uuid() const { return uuid_; };

private:
    buuid::uuid uuid_;
};

class AESKeyMessage : public ServerMessage {
public:
    AESKeyMessage(ServerVersion version, uint32_t payload_size, buuid::uuid uuid, string aes_key);

    static AESKeyMessage parse_from_incoming_message(shared_ptr<AbstractIncomingMessageReader> message,
                                                     ServerVersion expected_version);

    const buuid::uuid& get_uuid() const { return uuid_; };
    const string& get_encrypted_aes_key() const { return encrypted_aes_key_; };

private:
    buuid::uuid uuid_;
    string encrypted_aes_key_;
};

class UploadFileSuccessfulMessage : public ServerMessage {
public:
    UploadFileSuccessfulMessage(ServerVersion version, uint32_t payload_size, buuid::uuid uuid, uint32_t content_size, util::NameString filename, uint32_t checksum);

    static UploadFileSuccessfulMessage parse_from_incoming_message(shared_ptr<AbstractIncomingMessageReader> message,
                                                                   ServerVersion expected_version);

    const buuid::uuid& get_uuid() const { return uuid_; };
    const util::NameString& get_filename() const { return filename_; };
    uint32_t get_checksum() const { return checksum_; };
    uint32_t get_content_size() const { return content_size_; };

private:
    buuid::uuid uuid_;
    uint32_t content_size_;
    util::NameString filename_;
    uint32_t checksum_;
};

class SuccessResponseMessage : public ServerMessage {
public:
    SuccessResponseMessage(ServerVersion version, uint32_t payload_size);

    static SuccessResponseMessage parse_from_incoming_message(shared_ptr<AbstractIncomingMessageReader> message,
                                                              ServerVersion expected_version);
};

}  // namespace protocol
}  // namespace client
