#include "client.h"

#include <boost/log/trivial.hpp>
#include <boost/uuid/string_generator.hpp>

namespace basio = boost::asio;

namespace client {

Client::Client(UserInfo user_info, TransferInfo transfer_info, ConnectionManager connection,
               protocol::ClientVersion client_version, protocol::ServerVersion server_version)
    : user_info_(std::move(user_info)),
      transfer_info_(std::move(transfer_info)),
      connection_(std::move(connection)),
      client_version_(client_version),
      server_version_(server_version) {}

void Client::run() {
    BOOST_LOG_TRIVIAL(info) << "Starting client run";

    BOOST_LOG_TRIVIAL(info) << "Connecting to: " << transfer_info_.get_server();
    connection_.connect();

    if (!user_info_.does_file_exist()) {
        send_registration_request();
    }
}

void Client::send_registration_request() {
    BOOST_LOG_TRIVIAL(info) << "Sending registration request";
    buuid::string_generator gen;
    buuid::uuid default_uuid{gen("64f3f63985f04beb81a0e43321880182")};  // TODO: is this correct?
    protocol::RegistrationRequestMessage registration_request{client_version_,
                                                              default_uuid,
                                                              transfer_info_.get_client_name()};
    connection_.write(registration_request.pack());
    BOOST_LOG_TRIVIAL(info) << server_version_;
    // protocol::RegistrationSuccessfulMessage response{protocol::RegistrationSuccessfulMessage::parse_from_incoming_message()}
}

}  // namespace client
