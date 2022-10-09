#pragma once

#include <cstdint>
#include <string>

#include "connection_manager.h"

using std::string;

namespace client {

/**
 * @brief Abstract class to provide an interface to message parsers.
 * This allows us to unit test parsing messages from a buffer without
 * a connection for example
 *
 */
class AbstractIncomingMessageReader {
public:
    virtual ~AbstractIncomingMessageReader() = default;

    // All read functions expect the data to be sent in little endian
    virtual uint8_t read_uint8() = 0;
    virtual uint16_t read_uint16() = 0;
    virtual uint32_t read_uint32() = 0;
    virtual string read_bytes(size_t length) = 0;
};

class ConnectionManagerMessageReader : public AbstractIncomingMessageReader {
public:
    ConnectionManagerMessageReader(ConnectionManager& connection);
    virtual uint8_t read_uint8() override;
    virtual uint16_t read_uint16() override;
    virtual uint32_t read_uint32() override;
    virtual string read_bytes(size_t length) override;

private:
    ConnectionManager& connection_;
};

}  // namespace client
