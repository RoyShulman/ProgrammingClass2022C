#include "name_string.h"

namespace client {
namespace util {

StringLengthTooLarge::StringLengthTooLarge(const string& s, size_t max_size)
    : invalid_argument("String: " + s + " with size: " + std::to_string(s.size()) + " is larger than maximum: " + std::to_string(max_size)) {}

NameString::NameString(const string& s)
    : name_{} {
    if (s.size() > name_.size() - 1) {
        throw StringLengthTooLarge{s, name_.size() - 1};
    }

    // The array is initialized to '\0' so we just need to copy the string
    for (size_t i = 0; i < s.size(); i++) {
        name_[i] = s[i];
    }
}
}  // namespace util
}  // namespace client