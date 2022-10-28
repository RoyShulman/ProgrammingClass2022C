#include "string_utils.h"

#include <sstream>

using std::stringstream;

namespace client {
namespace util {

StringIsNotAValidEndpoint::StringIsNotAValidEndpoint(const string& string_endpoint)
    : invalid_argument("Given string: " + string_endpoint + " cannot be converted to boost endopoint") {}

bip::tcp::endpoint string_to_endpoint(const string& string_endpoint) {
    string ip, port;
    stringstream stream{string_endpoint};
    const char delimiter{':'};
    std::getline(stream, ip, delimiter);
    std::getline(stream, port, delimiter);

    boost::asio::io_service io_service;
    bip::tcp::resolver resolver{io_service};
    try {
        return {*resolver.resolve(ip, port)};
    } catch (const std::exception& e) {
        throw StringIsNotAValidEndpoint{string_endpoint};
    };
}

}  // namespace util
}  // namespace client
