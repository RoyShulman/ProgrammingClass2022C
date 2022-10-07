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
    void send_registration_request();

    UserInfo user_info_;
    TransferInfo transfer_info_;
    ConnectionManager connection_;
    protocol::ClientVersion client_version_;
    protocol::ServerVersion server_version_;
    shared_ptr<AbstractIncomingMessageReader> reader_;
};

}  // namespace client
