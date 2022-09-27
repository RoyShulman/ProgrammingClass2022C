#include "user_info.h"

#include <cryptopp/hex.h>
#include <cryptopp/rsa.h>

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "util/file.h"

namespace client {

const bfs::path UserInfo::DEFAULT_FILENAME_ = bfs::path("me.info");

InvalidUserInfoFile::InvalidUserInfoFile(const bfs::path& info_file, const string& error)
    : invalid_argument("Invalid user info file: " + info_file.string() + "\nError: " + error) {}

UserInfo::UserInfo() : name_{}, uuid_{}, key_{} {}

UserInfo::UserInfo(string name, buuid::uuid uuid, encryption::PrivateKeyWrapper key)
    : name_(std::move(name)), uuid_(std::move(uuid)), key_(std::move(key)) {}

UserInfo UserInfo::from_file(const bfs::path& info_file) {
    util::File from_file{info_file};
    vector<string> lines{from_file.read_lines()};
    if (lines.size() != NUM_LINES_IN_INFO_FILE_) {
        throw InvalidUserInfoFile(info_file, "Wrong number of lines in file");
    }

    buuid::string_generator string_generator;
    try {
        return UserInfo{lines[0],
                        string_generator(lines[1]),
                        encryption::PrivateKeyWrapper{lines[2]}};
    } catch (const std::exception& e) {
        throw InvalidUserInfoFile(info_file, e.what());
    }
}

void UserInfo::save_to_file(const bfs::path& info_file) const {
    std::ofstream file(info_file.string());
    file << name_ << std::endl;
    file << uuid_ << std::endl;
    file << key_ << std::endl;
}

bool UserInfo::operator==(const UserInfo& rhs) const {
    return (name_ == rhs.name_) && (uuid_ == rhs.uuid_) && (key_ == rhs.key_);
}

bool UserInfo::operator!=(const UserInfo& rhs) const {
    return !operator==(rhs);
}

std::ostream& operator<<(std::ostream& stream, const UserInfo& info) {
    return stream << "UserInfo:\n"
                  << "name: " << info.name_
                  << "\nuuid: " << buuid::to_string(info.uuid_)
                  << "\nkey: " << info.key_;
}

}  // namespace client
