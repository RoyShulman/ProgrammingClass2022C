#include "string_utils.h"

#include <sstream>

using std::stringstream;

namespace client {
namespace util {

StringIsNotAValidEndpoint::StringIsNotAValidEndpoint(const string& s)
    : invalid_argument("Given string: " + s + " cannot be converted to boost endopoint") {}

bip::tcp::endpoint string_to_endpoint(const string& s) {
    string ip, port;
    stringstream stream{s};
    const char delimiter{':'};
    std::getline(stream, ip, delimiter);
    std::getline(stream, port, delimiter);

    boost::asio::io_service io_service;
    bip::tcp::resolver resolver{io_service};
    try {
        return {*resolver.resolve(ip, port)};
    } catch (const std::exception& e) {
        throw StringIsNotAValidEndpoint{s};
    };
}

}  // namespace util
}  // namespace client
