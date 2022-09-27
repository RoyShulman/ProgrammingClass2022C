#include <boost/log/trivial.hpp>
#include <exception>

#include "client.h"

int main() {
    try {
        client::UserInfo user_info;
        client::protocol::ClientVersion client_version{3};
        client::protocol::ServerVersion server_version{3};
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
