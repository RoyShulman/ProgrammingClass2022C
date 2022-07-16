#pragma once

#include <boost/asio.hpp>
#include <memory>

#include "connection_manager.h"

using boost::asio::ip::tcp;
using std::unique_ptr;
class BoostConnectionManager : public AbstractConnectionManager {
public:
    BoostConnectionManager(unique_ptr<tcp::socket> client_socket);
    virtual void send(const vector<uint8_t>& to_send) override;
    virtual vector<uint8_t> recv(size_t size) override;

private:
    unique_ptr<tcp::socket> client_socket_;
};
