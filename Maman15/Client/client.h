#pragma once

#include <boost/asio.hpp>
#include <memory>

#include "connection_manager.h"
#include "protocol/client_message.h"
#include "protocol/server_message.h"
#include "transfer_info.h"
#include "user_info.h"

using boost::asio::ip::tcp;

namespace client {

class Client {
public:
    Client(UserInfo user_info, TransferInfo transfer_info, ConnectionManager connection,
           protocol::ClientVersion client_version, protocol::ServerVersion server_version);

    void run();

private:
    /**
     * @brief Register the client, and update the user info file
     *
     * @return true if registration was successful, false otherwise
     */
    bool register_client();

    /**
     * @brief Send the public key to the server, and accept the encrypted aes eky
     *
     * @return The aes key
     */
    string exchange_keys();

    /**
     * @brief Encrypt the file with the aes key, send it and validate the checksum. We retry NUM_SEND_FILE_RETRIES times
     *
     * @return true If the file was sent and the checksum was validated
     */
    bool send_encrypted_file_and_validate_checksum(const string& aes_key);

    /**
     * @brief Encrypt the content of the transfer file with the aes key
     *
     * @return encrypted content
     */
    string get_encrypted_transfer_file(const string& aes_key) const;

    uint32_t get_transfer_file_checksum() const;

    static constexpr size_t PUBLIC_KEY_SIZE = 1024;
    static constexpr size_t NUM_SEND_FILE_RETRIES = 3;

    UserInfo user_info_;
    TransferInfo transfer_info_;
    ConnectionManager connection_;
    protocol::ClientVersion client_version_;
    protocol::ServerVersion server_version_;
    shared_ptr<AbstractIncomingMessageReader> reader_;
};

}  // namespace client
