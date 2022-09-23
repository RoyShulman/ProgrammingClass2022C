#pragma once

#include <cstdint>
#include <string>

using std::string;

namespace client {
namespace util {
/**
 * @brief Class to write values in little endian to a given
 * string reference.
 *
 */
class LittleEndianStringStream {
public:
    // The constructor doesn't accept a `const string&` because we want it to be mutable
    explicit LittleEndianStringStream(string& container);

    LittleEndianStringStream& operator<<(uint8_t value);
    LittleEndianStringStream& operator<<(uint16_t value);
    LittleEndianStringStream& operator<<(uint32_t value);
    LittleEndianStringStream& operator<<(const string& value);

private:
    string& container_;
};

}  // namespace util
}  // namespace client
