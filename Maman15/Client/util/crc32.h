#pragma once
#include <cstdint>
#include <string>

using std::string;

namespace client {
namespace util {

/**
 * @brief Calculate crc32 on the given buffer
 * 
 * @param buffer - To calculate crc32 on
 * @return uint32_t The crc32 result
 */
uint32_t crc32(const string& buffer);

}
}  // namespace client
