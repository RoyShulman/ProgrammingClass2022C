#include "client.h"

#include <boost/log/trivial.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "encryption/aes_wrapper.h"
#include "util/crc32.h"
#include "util/file.h"

namespace basio = boost::asio;
using std::make_shared;

namespace client {

Client::Client(UserInfo user_info, TransferInfo transfer_info, ConnectionManager connection,
               protocol::ClientVersion client_version, protocol::ServerVersion server_version)
    : user_info_(std::move(user_info)),
      transfer_info_(std::move(transfer_info)),
      connection_(std::move(connection)),
      client_version_(client_version),
      server_version_(server_version),
      reader_{make_shared<client::ConnectionManagerMessageReader>(connection_)} {}

void Client::run() {
    BOOST_LOG_TRIVIAL(info) << "Starting client run";

    BOOST_LOG_TRIVIAL(info) << "Connecting to: " << transfer_info_.get_server();
    connection_.connect();

    if (!user_info_.does_file_exist()) {
        if (!register_client()) {
            return;
        }
    } else {
        user_info_ = user_info_.from_file();
    }

    string aes_key{exchange_keys()};
    if (send_encrypted_file_and_validate_checksum(aes_key)) {
        BOOST_LOG_TRIVIAL(info) << "File uploaded successfuly";
        protocol::FileCRCOkMessage message{client_version_,
                                           user_info_.get_uuid(),
                                           transfer_info_.get_transfer_file().string()};
        connection_.write(message.pack());
    } else {
        BOOST_LOG_TRIVIAL(error) << "File could not be uploaded";
        protocol::CRCIncorrectGivingUp message{client_version_,
                                               user_info_.get_uuid(),
                                               transfer_info_.get_transfer_file().string()};
        connection_.write(message.pack());
        return;
    }
    auto response{protocol::SuccessResponseMessage::parse_from_incoming_message(reader_, server_version_)};
    BOOST_LOG_TRIVIAL(info) << "Server sent final success response. Exiting";
}

bool Client::register_client() {
    BOOST_LOG_TRIVIAL(info) << "Sending registration request";
    buuid::string_generator gen;
    // The initial uuid should be 0. When we register the server sends us the new uuid
    buuid::uuid default_uuid{gen("00000000000000000000000000000000")};
    protocol::RegistrationRequestMessage registration_request{client_version_,
                                                              default_uuid,
                                                              transfer_info_.get_client_name()};
    connection_.write(registration_request.pack());
    try {
        protocol::RegistrationSuccessfulMessage response{protocol::RegistrationSuccessfulMessage::parse_from_incoming_message(reader_,
                                                                                                                              server_version_)};
        BOOST_LOG_TRIVIAL(info) << "Successful registration. Using new uuid: " << buuid::to_string(response.get_uuid());
        user_info_.set_uuid(response.get_uuid());
        user_info_.set_name(transfer_info_.get_client_name());
        user_info_.set_key(encryption::PrivateKeyWrapper(PUBLIC_KEY_SIZE));
        user_info_.save_to_file();
        BOOST_LOG_TRIVIAL(info) << "Saved new user info: " << user_info_;
    } catch (const protocol::RegistrationFailedException& e) {
        BOOST_LOG_TRIVIAL(error) << "Registration failed";
        return false;
    }
    return true;
}

string Client::exchange_keys() {
    BOOST_LOG_TRIVIAL(info) << "Exchanging keys";
    protocol::ClientPublicKeyMessage public_key_message{client_version_,
                                                        user_info_.get_uuid(),
                                                        transfer_info_.get_client_name(),
                                                        user_info_.get_key().get_public()};
    connection_.write(public_key_message.pack());
    protocol::AESKeyMessage aes_key_message{protocol::AESKeyMessage::parse_from_incoming_message(reader_,
                                                                                                 server_version_)};
    string encrypted_aes{aes_key_message.get_encrypted_aes_key()};
    return user_info_.get_key().decrypt(encrypted_aes);
}

bool Client::send_encrypted_file_and_validate_checksum(const string& aes_key) {
    BOOST_LOG_TRIVIAL(info) << "Uploading file: " << transfer_info_.get_transfer_file();
    string encrypted_content{get_encrypted_transfer_file(aes_key)};
    protocol::UploadFileMessage message{client_version_,
                                        user_info_.get_uuid(),
                                        transfer_info_.get_transfer_file().string(),
                                        encrypted_content};
    uint32_t checksum{get_transfer_file_checksum()};
    BOOST_LOG_TRIVIAL(debug) << "File checkusm is: " << std::to_string(checksum);

    bool validated = false;
    for (size_t i = 0; i < NUM_SEND_FILE_RETRIES; i++) {
        connection_.write(message.pack());
        auto response{protocol::UploadFileSuccessfulMessage::parse_from_incoming_message(reader_,
                                                                                         server_version_)};
        if (response.get_checksum() != checksum && i != NUM_SEND_FILE_RETRIES - 1) {
            // We don't want to send a retry on the last message
            BOOST_LOG_TRIVIAL(error) << "Incorrect checksum. Retrying (retry number: " << std::to_string(i) << ")";
            protocol::CRCIncorrectWillRetry retry_message{client_version_,
                                                          user_info_.get_uuid(),
                                                          transfer_info_.get_transfer_file().string()};
            connection_.write(retry_message.pack());
        } else {
            validated = true;
            break;
        }
    }

    if (validated) {
        return true;
    }
    return false;
}

string Client::get_encrypted_transfer_file(const string& aes_key) const {
    util::File transfer_file{transfer_info_.get_transfer_file(), true};
    string content{transfer_file.read()};
    encryption::AESWrapper aes{aes_key};
    return aes.encrypt(content);
}

uint32_t Client::get_transfer_file_checksum() const {
    util::File transfer_file{transfer_info_.get_transfer_file(), true};
    string content{transfer_file.read()};
    return util::crc32(content);
}

}  // namespace client
