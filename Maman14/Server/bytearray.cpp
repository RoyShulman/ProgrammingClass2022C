#include "bytearray.h"

namespace utils {

void Bytearray::push_u8(uint8_t value) {
    buffer_.push_back(value);
}

void Bytearray::push_u16(uint16_t value) {
    // We want to pack in little endian
    uint8_t lower = value & 0xff;
    uint8_t higher = value >> 8;
    buffer_.push_back(lower);
    buffer_.push_back(higher);
}

void Bytearray::push_u32(uint32_t value) {
    buffer_.push_back(value & 0xff);
    buffer_.push_back((value >> 8) & 0xff);
    buffer_.push_back((value >> 16) & 0xff);
    buffer_.push_back((value >> 24) & 0xff);
}

void Bytearray::push_string(const string& value) {
    // Ignore the null terminator
    for (size_t i = 0; i < value.size(); i++) {
        buffer_.push_back(value[i]);
    }
}

void Bytearray::push_bytes(const Bytearray& value) {
    buffer_.insert(buffer_.end(), value.buffer_.begin(), value.buffer_.end());
}

void Bytearray::push_vector(const vector<uint8_t>& vec) {
    buffer_.insert(buffer_.end(), vec.begin(), vec.end());
}

}  // namespace utils
