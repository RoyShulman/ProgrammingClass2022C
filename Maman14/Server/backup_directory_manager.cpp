#include "backup_directory_manager.h"

#include <iostream>
#include <string>

using std::lock_guard;

BackupDirectoryForUserNotFound::BackupDirectoryForUserNotFound(user_id_t user_id)
    : runtime_error("Backup directory for user ID: " + std::to_string(user_id) + " Not found"), user_id(user_id) {
}

BackupDirectoryManager::BackupDirectoryManager(bfs::path root_backup_directory)
    : root_backup_directory_(std::move(root_backup_directory)) {
    bfs::create_directory(root_backup_directory_);
}

void BackupDirectoryManager::backup_file_for_user_id(user_id_t user_id, const string& filename, const vector<uint8_t>& payload) {
    lock_guard<mutex> lock(mutex_);
    auto& user_dir = get_or_add_user(user_id);
    user_dir.backup_file(filename, payload);
}

const vector<string> BackupDirectoryManager::get_backup_filenames_for_user(user_id_t user_id) const {
    lock_guard<mutex> lock(mutex_);
    const auto& user_dir = get_user_directory(user_id);
    return user_dir.get_backup_filenames();
}

const vector<uint8_t> BackupDirectoryManager::get_file_content_for_user(user_id_t user_id, const string& filename) const {
    lock_guard<mutex> lock(mutex_);
    const auto& user_dir = get_user_directory(user_id);
    return user_dir.get_backup_file_content(filename);
}

void BackupDirectoryManager::delete_file_for_user(user_id_t user_id, const string& filename) {
    lock_guard<mutex> lock(mutex_);
    auto& user_dir = get_mutable_user_directory(user_id);
    user_dir.delete_file(filename);
}

UserBackupDirectory& BackupDirectoryManager::get_or_add_user(user_id_t user_id) {
    auto it = user_directories_.find(user_id);
    if (it != user_directories_.end()) {
        return it->second;
    }

    auto user_directory = root_backup_directory_ / std::to_string(user_id);
    bfs::create_directory(user_directory);
    user_directories_.emplace(user_id, user_directory);
    return user_directories_.find(user_id)->second;
}

const UserBackupDirectory& BackupDirectoryManager::get_user_directory(user_id_t user_id) const {
    auto it = user_directories_.find(user_id);
    if (it == user_directories_.end()) {
        throw BackupDirectoryForUserNotFound(user_id);
    }
    return it->second;
}

UserBackupDirectory& BackupDirectoryManager::get_mutable_user_directory(user_id_t user_id) {
    auto it = user_directories_.find(user_id);
    if (it == user_directories_.end()) {
        throw BackupDirectoryForUserNotFound(user_id);
    }
    return it->second;
}
