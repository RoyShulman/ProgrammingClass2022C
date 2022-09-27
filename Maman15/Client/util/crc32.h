#pragma once
#include <cstdint>
#include <string>

using std::string;

namespace client {
namespace util {

uint32_t crc32(const string& buffer);

}
}  // namespace client
