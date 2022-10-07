#include "incoming_message_reader.h"

namespace client {

ConnectionManagerMessageReader::ConnectionManagerMessageReader(ConnectionManager& connection)
    : connection_(connection) {}

uint8_t ConnectionManagerMessageReader::read_uint8() {
    return connection_.read(1)[0];
}

uint16_t ConnectionManagerMessageReader::read_uint16() {
    string read_value{connection_.read(2)};
    return read_value[0] | read_value[1] << 8;
}
uint32_t ConnectionManagerMessageReader::read_uint32() {
    string read_value{connection_.read(4)};
    uint32_t return_value = read_value[0];
    return_value |= read_value[1] << 8;
    return_value |= read_value[2] << 16;
    return_value |= read_value[3] << 24;
    return return_value;
}

string ConnectionManagerMessageReader::read_bytes(size_t length) {
    return connection_.read(length);
}

}  // namespace client
