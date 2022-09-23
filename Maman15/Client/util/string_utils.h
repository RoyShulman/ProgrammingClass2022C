#pragma once
#include <string>
#include <boost/asio.hpp>
#include <stdexcept>

namespace bip = boost::asio::ip;
using std::string;
using std::invalid_argument;

namespace client {
namespace util {

class StringIsNotAValidEndpoint : public invalid_argument {
public:
    explicit StringIsNotAValidEndpoint(const string& s);
};

bip::tcp::endpoint string_to_endpoint(const string& s);

}
}  // namespace client
