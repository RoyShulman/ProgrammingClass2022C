#include "request_reader.h"

RequestReader::RequestReader(shared_ptr<AbstractConnectionManager> connection)
    : connection_(connection) {}

uint32_t RequestReader::read_uint32() {
    vector<uint8_t> buffer{connection_->recv(4)};
    return buffer.at(3) << 24 | buffer.at(2) << 16 | buffer.at(1) << 8 | buffer.at(0);
}

uint16_t RequestReader::read_uint16() {
    vector<uint8_t> buffer{connection_->recv(2)};
    return buffer.at(1) << 8 | buffer.at(0);
}

uint8_t RequestReader::read_uint8() {
    return connection_->recv(1).at(0);
}

vector<uint8_t> RequestReader::read_bytes(size_t size) {
    return connection_->recv(size);
}
