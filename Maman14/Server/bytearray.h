#pragma once
#include <cstdint>
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace utils {

class Bytearray {
public:
    void push_u8(uint8_t value);
    void push_u16(uint16_t value);
    void push_u32(uint32_t value);
    void push_string(const string& value);
    void push_bytes(const Bytearray& value);
    void push_vector(const vector<uint8_t>& vec);

    const uint8_t* data() const { return buffer_.data(); };
    uint32_t len() const { return buffer_.size(); };
    const vector<uint8_t>& buffer() const { return buffer_; };

private:
    vector<uint8_t> buffer_;
};
}  // namespace utils
