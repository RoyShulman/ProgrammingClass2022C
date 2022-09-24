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
public:
    UserInfo(string name, buuid::uuid uuid, encryption::PrivateKeyWrapper key);

    static UserInfo from_file(const bfs::path& info_file);

    bool operator==(const UserInfo& rhs) const;
    bool operator!=(const UserInfo& rhs) const;
    friend std::ostream& operator<<(std::ostream& stream, const UserInfo& info);

private:
    static const size_t NUM_LINES_IN_INFO_FILE_ = 3;

    string name_;
    buuid::uuid uuid_;
    encryption::PrivateKeyWrapper key_;
};

}  // namespace client
