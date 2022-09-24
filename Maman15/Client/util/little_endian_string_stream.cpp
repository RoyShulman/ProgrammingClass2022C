#include "little_endian_string_stream.h"

#include <sstream>

namespace client {
namespace util {

LittleEndianStringStream::LittleEndianStringStream(string& container)
    : container_(container) {}

LittleEndianStringStream& LittleEndianStringStream::operator<<(uint8_t value) {
    container_.push_back(value);
    return *this;
}

LittleEndianStringStream& LittleEndianStringStream::operator<<(uint16_t value) {
    const uint8_t lower = value & 0xff;
    const uint8_t higher = (value >> 8) & 0xf;
    *this << lower << higher;
    return *this;
}

LittleEndianStringStream& LittleEndianStringStream::operator<<(uint32_t value) {
    const uint8_t lower = value & 0xff;
    const uint8_t bytes16 = (value >> 8) & 0xff;
    const uint8_t bytes24 = (value >> 16) & 0xff;
    const uint8_t higher = (value >> 24) & 0xff;
    *this << lower << bytes16 << bytes24 << higher;
    return *this;
}

LittleEndianStringStream& LittleEndianStringStream::operator<<(const string& value) {
    container_.append(value);
    return *this;
}

LittleEndianStringStream& LittleEndianStringStream::operator<<(const buuid::uuid& value) {
    std::stringstream stream;
    for (size_t i = 0; i < sizeof(value.data); i++) {
        stream << value.data[i];
    }
    container_.append(stream.str());
    return *this;
}

}  // namespace util
}  // namespace client
