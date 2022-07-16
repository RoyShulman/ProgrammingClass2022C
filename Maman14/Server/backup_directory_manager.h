#pragma once

#include <boost/filesystem.hpp>
#include <exception>
#include <map>
#include <mutex>
#include <string>

#include "user_backup_directory.h"

namespace bfs = boost::filesystem;
using std::map;
using std::mutex;
using std::runtime_error;
using std::string;

typedef uint32_t user_id_t;

class BackupDirectoryForUserNotFound : public runtime_error {
public:
    BackupDirectoryForUserNotFound(user_id_t user_id);

    user_id_t user_id;
};

class BackupDirectoryManager {
public:
    BackupDirectoryManager(bfs::path root_backup_directory = bfs::temp_directory_path());

    void backup_file_for_user_id(user_id_t user_id, const string& filename, const vector<uint8_t>& payload);

    /**
     * @brief Get the number of backup directories. This should equal the number of user ID's seens o far
     *
     * @return size_t Number of backup directories
     */
    size_t get_num_backup_directories() const { return user_directories_.size(); };

    const vector<string> get_backup_filenames_for_user(user_id_t user_id) const;

    const vector<uint8_t> get_file_content_for_user(user_id_t user_id, const string& filename) const;

    void delete_file_for_user(user_id_t user_id, const string& filename);

    const bfs::path& get_root_backup_directory() const { return root_backup_directory_; };

private:
    /**
     * @brief Get or add a UserBackupDirectory to the user_directories_ map.
     * It's ok to return a reference here since the function is private and the objects lifetime
     * will be less than or equal to the lifetime of this containing object
     *
     * @param user_id - The user's ID
     * @return UserBackupDirectory& - The user's backup directory
     */
    UserBackupDirectory& get_or_add_user(user_id_t user_id);

    /**
     * @brief Get the UserBackupDirectory for the specified user id.
     * Will throw if a directory doesn't exists.
     * Again ok to return reference.
     *
     * @param user_id - The user's ID
     * @return UserBackupDirectory& - The user's backup directory
     */
    const UserBackupDirectory& get_user_directory(user_id_t user_id) const;

    UserBackupDirectory& get_mutable_user_directory(user_id_t user_id);

    map<user_id_t, UserBackupDirectory> user_directories_;
    bfs::path root_backup_directory_;
    mutable mutex mutex_;
};
