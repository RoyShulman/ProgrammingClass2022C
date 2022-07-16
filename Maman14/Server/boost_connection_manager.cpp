#include "boost_connection_manager.h"

BoostConnectionManager::BoostConnectionManager(unique_ptr<tcp::socket> client_socket)
    : client_socket_(std::move(client_socket)) {}

void BoostConnectionManager::send(const vector<uint8_t>& to_send) {
    boost::asio::write(*client_socket_, boost::asio::buffer(to_send));
}

vector<uint8_t> BoostConnectionManager::recv(size_t size) {
    vector<uint8_t> recv_buf(size);
    boost::asio::read(*client_socket_, boost::asio::buffer(recv_buf));
    return recv_buf;
}
