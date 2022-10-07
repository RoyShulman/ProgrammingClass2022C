#include "client.h"

#include <boost/log/trivial.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

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
        send_registration_request();
    }
}

void Client::send_registration_request() {
    BOOST_LOG_TRIVIAL(info) << "Sending registration request";
    buuid::string_generator gen;
    // The initial uuid should be 0. When we register the server sends us the new uuid
    buuid::uuid default_uuid{gen("00000000000000000000000000000000")};
    protocol::RegistrationRequestMessage registration_request{client_version_,
                                                              default_uuid,
                                                              transfer_info_.get_client_name()};
    connection_.write(registration_request.pack());
    BOOST_LOG_TRIVIAL(info) << server_version_;
    protocol::RegistrationSuccessfulMessage response{protocol::RegistrationSuccessfulMessage::parse_from_incoming_message(reader_,
                                                                                                                          server_version_)};
    BOOST_LOG_TRIVIAL(info) << "Successful registration. Using new uuid: " << buuid::to_string(response.get_uuid());
}

}  // namespace client
