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
    explicit StringIsNotAValidEndpoint(const string& string_endpoint);
};

/**
 * @brief Convert a string to the tcp endpoint
 * 
 * @param string_endpoint - The string to convert
 * @return bip::tcp::endpoint 
 */
bip::tcp::endpoint string_to_endpoint(const string& string_endpoint);

}
}  // namespace client
