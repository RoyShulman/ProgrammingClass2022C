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

/**
 * @brief Stores information about the user. This fields include:
 * name, uuid and private key.
 *
 */
class UserInfo {
private:
    static const size_t NUM_LINES_IN_INFO_FILE_ = 3;
    static const bfs::path DEFAULT_FILENAME_;

    string name_;
    buuid::uuid uuid_;
    encryption::PrivateKeyWrapper key_;

public:
    UserInfo();
    UserInfo(string name, buuid::uuid uuid, encryption::PrivateKeyWrapper key);

    /**
     * @brief Read the info from the given file
     * 
     * @param info_file - The filename to read from
     * @return UserInfo 
     */
    static UserInfo from_file(const bfs::path& info_file = DEFAULT_FILENAME_);

    /**
     * @brief Check if the default file to read from exists
     * 
     * @return bool
     */
    bool does_file_exist() const { return bfs::exists(DEFAULT_FILENAME_); };

    /**
     * @brief Save the user information to the given file
     * 
     * @param info_file - Filename to save to
     */
    void save_to_file(const bfs::path& info_file = DEFAULT_FILENAME_) const;

    void set_name(string name) { name_ = std::move(name); };
    void set_uuid(buuid::uuid uuid) { uuid_ = std::move(uuid); };
    void set_key(encryption::PrivateKeyWrapper key) { key_ = std::move(key); };

    const string& get_name() const { return name_; };
    const buuid::uuid& get_uuid() const { return uuid_; };
    const encryption::PrivateKeyWrapper& get_key() const { return key_; };

    bool operator==(const UserInfo& rhs) const;
    bool operator!=(const UserInfo& rhs) const;
    friend std::ostream& operator<<(std::ostream& stream, const UserInfo& info);
};

}  // namespace client
