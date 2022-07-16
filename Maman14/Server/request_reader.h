#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "connection_manager.h"

using std::shared_ptr;
using std::vector;

/**
 * @brief Abstract class, that reads values that are sent in little endian,
 * from an interface that implements recv
 *
 */
class AbstractRequestReader {
public:
    /**
     * @brief Read a uint32 that was sent in little endian
     *
     * @return uint32_t The value
     */
    virtual uint32_t read_uint32() = 0;

    /**
     * @brief Read a uint16 that was sent in little endian
     *
     * @return uint16_t The value
     */
    virtual uint16_t read_uint16() = 0;

    /**
     * @brief Read a uint8
     *
     * @return uint8_t The value
     */
    virtual uint8_t read_uint8() = 0;

    /**
     * @brief Read a number of size bytes
     *
     * @param size Size to read
     * @return vector<uint8_t> The return bytes
     */
    virtual vector<uint8_t> read_bytes(size_t size) = 0;

    virtual ~AbstractRequestReader() = default;

protected:
    AbstractRequestReader() = default;
};

class RequestReader : public AbstractRequestReader {
public:
    RequestReader(shared_ptr<AbstractConnectionManager> connection);

    virtual uint32_t read_uint32();
    virtual uint16_t read_uint16();
    virtual uint8_t read_uint8();
    virtual vector<uint8_t> read_bytes(size_t size);

private:
    // We want the connection to live for as long as the request reader
    shared_ptr<AbstractConnectionManager> connection_;
};
