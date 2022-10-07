#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <string>

namespace basio = boost::asio;
namespace bip = boost::asio::ip;
using std::string;
using std::unique_ptr;

namespace client {

class ConnectionManager {
public:
    ConnectionManager(bip::tcp::endpoint server);

    void connect();

    void write(const string& buffer);
    string read(size_t size);

private:
    unique_ptr<basio::io_context> io_context_;
    bip::tcp::endpoint server_;
    bip::tcp::resolver resolver_;
    bip::tcp::socket socket_;
};

}  // namespace client
