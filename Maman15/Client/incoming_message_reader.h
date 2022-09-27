#pragma once

#include <cstdint>
#include <string>

using std::string;

namespace client {

/**
 * @brief Abstract class to provide an interface to message parsers.
 * This allows us to unit tests parsing messages from a buffer without
 * a connection for example
 *
 */
class AbstractIncomingMessageReader {
public:
    virtual ~AbstractIncomingMessageReader() = default;

    virtual uint8_t read_uint8() = 0;
    virtual uint16_t read_uint16() = 0;
    virtual uint32_t read_uint32() = 0;
    virtual string read_bytes(size_t length) = 0;
};

}  // namespace client
