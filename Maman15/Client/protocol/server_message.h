#pragma once
#include <boost/uuid/uuid.hpp>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>

#include "../incoming_message_reader.h"
#include "../util/name_string.h"

namespace buuid = boost::uuids;
using std::runtime_error;
using std::shared_ptr;
using std::string;

namespace client {
namespace protocol {

typedef uint8_t ServerVersion;

enum class ServerMessageID : uint16_t {
    REGISTRATION_SUCCESSFUL = 2100,
    AES_KEY = 2102,
    UPLOAD_FILE_SUCCESSFUL = 2103,
    SUCCESS_RESPONSE = 2104,  // TODO: what about this??
};

class WrongMessageVersion : public runtime_error {
public:
    WrongMessageVersion(ServerVersion expected, ServerVersion read);
};

class WrongMessageCode : public runtime_error {
public:
    WrongMessageCode(ServerMessageID expected, ServerMessageID read);
};

class FailedToParseMessage : public runtime_error {
public:
    FailedToParseMessage(const string& message, const string& error);
};

/**
 * @brief Abstract base class for all server messages.
 */
class ServerMessage {
protected:
    ServerMessage(ServerVersion version, ServerMessageID code, buuid::uuid uuid);

public:
    virtual ~ServerMessage() = default;

    static ServerMessage parse_header_from_incoming_message(shared_ptr<AbstractIncomingMessageReader> message,
                                                            ServerMessageID expected_code, ServerVersion expected_version);

    ServerVersion get_version() const { return version_; };
    const buuid::uuid& get_uuid() const { return uuid_; };
    ServerMessageID get_code() const { return code_; };

private:
    ServerVersion version_;
    ServerMessageID code_;
    buuid::uuid uuid_;
};

// All the following messages can parse from an incoming message.
// They should accept a pointer to an AbstractIncomingMessageReader to allow for polymorphism,
// parse_from_incoming_message isn't virtual since we want to return the concrete type of the message
class RegistrationSuccessfulMessage : public ServerMessage {
public:
    RegistrationSuccessfulMessage(ServerVersion version, buuid::uuid uuid);

    static RegistrationSuccessfulMessage parse_from_incoming_message(shared_ptr<AbstractIncomingMessageReader> message,
                                                                     ServerVersion expected_version);
};

class AESKeyMessage : public ServerMessage {
public:
    AESKeyMessage(ServerVersion version, buuid::uuid uuid, string aes_key);

    static AESKeyMessage parse_from_incoming_message(shared_ptr<AbstractIncomingMessageReader> message,
                                                     ServerVersion expected_version);

    const string& get_aes_key() const { return aes_key_; };

private:
    static constexpr size_t AES_KEY_SIZE_ = 16;
    string aes_key_;
};

class UploadFileSuccessfulMessage : public ServerMessage {
public:
    UploadFileSuccessfulMessage(ServerVersion version, buuid::uuid uuid, util::NameString filename, uint32_t checksum);

    static UploadFileSuccessfulMessage parse_from_incoming_message(shared_ptr<AbstractIncomingMessageReader> message,
                                                                   ServerVersion expected_version);

    const util::NameString& get_filename() const { return filename_; };
    uint32_t get_checksum() const { return checksum_; };

private:
    util::NameString filename_;
    uint32_t checksum_;
};

class SuccessResponseMessage : public ServerMessage {
public:
    SuccessResponseMessage(ServerVersion version, buuid::uuid uuid);

    static SuccessResponseMessage parse_from_incoming_message(shared_ptr<AbstractIncomingMessageReader> message,
                                                              ServerVersion expected_version);
};

}  // namespace protocol
}  // namespace client
