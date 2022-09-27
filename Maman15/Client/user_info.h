#pragma once

#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include <stdexcept>
#include <string>

#include "encryption/private_key_wrapper.h"

namespace bfs = boost::filesystem;
namespace buuid = boost::uuids;
using std::invalid_argument;
using std::string;

namespace client {

class InvalidUserInfoFile : public invalid_argument {
public:
    InvalidUserInfoFile(const bfs::path& info_file, const string& error);
};

class UserInfo {
private:
    static const size_t NUM_LINES_IN_INFO_FILE_ = 3;
    static const bfs::path DEFAULT_FILENAME_;

    string name_;
    buuid::uuid uuid_;
    encryption::PrivateKeyWrapper key_;

public:
    UserInfo(string name, buuid::uuid uuid, encryption::PrivateKeyWrapper key);

    static UserInfo from_file(const bfs::path& info_file = DEFAULT_FILENAME_);

    bool does_file_exist() const { return bfs::exists(DEFAULT_FILENAME_); };

    void save_to_file(const bfs::path& info_file = DEFAULT_FILENAME_) const;

    bool operator==(const UserInfo& rhs) const;
    bool operator!=(const UserInfo& rhs) const;
    friend std::ostream& operator<<(std::ostream& stream, const UserInfo& info);
};

}  // namespace client
