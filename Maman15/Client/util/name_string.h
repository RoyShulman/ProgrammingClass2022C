#pragma once
#include <array>
#include <cstddef>
#include <stdexcept>
#include <string>

using std::array;
using std::invalid_argument;
using std::string;

namespace client {
namespace util {

class StringLengthTooLarge : public invalid_argument {
public:
    StringLengthTooLarge(const string& s, size_t max_size);
};

/**
 * @brief Utility class for working with strings of a fixed length.
 * In the protocol we always send the name as 255 characters so this class helps
 * us work with strings of that constant size
 *
 */
class NameString {
private:
    static constexpr size_t NAME_SIZE_ = 255;
    typedef array<char, NAME_SIZE_> name_array;
    name_array name_;

public:
    // We don't want the constructor to be explicit because we want an implicit conversion from string
    NameString(const string& s);

    const name_array& get_name() const { return name_; };
    static constexpr size_t get_name_size() { return NAME_SIZE_; };

    bool operator==(const NameString& rhs) const;
    bool operator!=(const NameString& rhs) const;
};

}  // namespace util
}  // namespace client
