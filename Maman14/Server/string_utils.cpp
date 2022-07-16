#include "string_utils.h"

namespace utils {
string generate_random_alphanumeric(size_t length) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    string s;
    s.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return s;
}
}  // namespace utils
