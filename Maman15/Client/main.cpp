#include <boost/log/trivial.hpp>
#include <exception>

#include "client.h"

using std::make_shared;

static const uint8_t CLIENT_VERSION = 3;
static const uint8_t SERVER_VERSION = 3;

/**
 * @brief Main client functions.
 * Reads the transfer information and user information, connects
 * to the server, registers if it isn't already and trasnfers the file
 *
 * @return int
 */
int main() {
    try {
        client::UserInfo user_info;
        client::protocol::ClientVersion client_version{CLIENT_VERSION};
        client::protocol::ServerVersion server_version{SERVER_VERSION};
        client::TransferInfo transfer_info{client::TransferInfo::from_file()};
        client::ConnectionManager connection{transfer_info.get_server()};

        client::Client client(user_info, transfer_info, std::move(connection), client_version, server_version);
        client.run();
    } catch (const std::exception& error) {
        BOOST_LOG_TRIVIAL(fatal) << "Terminating with error: " << error.what();
        return 1;
    } catch (...) {
        BOOST_LOG_TRIVIAL(fatal) << "Terminating with unknown excpetion";
        return 2;
    }
    return 0;
}
