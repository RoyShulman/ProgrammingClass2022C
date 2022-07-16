#include "user_backup_directory.h"

#include <fstream>
#include <iostream>
#include <mutex>

using std::lock_guard;

FilePathException::FilePathException(string what, bfs::path full_path)
    : runtime_error(std::move(what)), filename_(full_path.filename().string()), full_path_(std::move(full_path)) {}

FileAlreadyExistsException::FileAlreadyExistsException(bfs::path full_path)
    : FilePathException("Filename: " + full_path.string() + " already exists", std::move(full_path)) {}

FileNotFoundException::FileNotFoundException(bfs::path full_path)
    : FilePathException("Filename: " + full_path.string() + " not found", std::move(full_path)) {}

FailedToDeleteFileException::FailedToDeleteFileException(bfs::path full_path)
    : FilePathException("Failed to delete: " + full_path.string(), std::move(full_path)) {}

UserBackupDirectory::UserBackupDirectory(bfs::path directory)
    : directory_(std::move(directory)) {}

static void write_to_file(const bfs::path file, const vector<uint8_t>& payload) {
    std::ofstream ofs(file.string(), std::ios::binary | std::ios::out);
    for (const auto& c : payload) {
        ofs << c;
    }
}

static const vector<uint8_t> read_from_file(const bfs::path file) {
    std::ifstream ifs(file.string(), std::ios::binary | std::ios::in);
    vector<unsigned char> content(std::istreambuf_iterator<char>(ifs), {});
    return content;
}

void UserBackupDirectory::backup_file(const string& filename, const vector<uint8_t>& payload) {
    lock_guard<mutex> lock(mutex_);
    bfs::path backup_file = directory_ / filename;
    if (bfs::exists(backup_file)) {
        throw FileAlreadyExistsException(backup_file);
    }

    write_to_file(backup_file, payload);
}

const vector<uint8_t> UserBackupDirectory::get_backup_file_content(const string& filename) const {
    lock_guard<mutex> lock(mutex_);
    bfs::path backup_file = directory_ / filename;
    if (!bfs::exists(backup_file)) {
        throw FileNotFoundException(backup_file);
    }
    return read_from_file(backup_file);
}

const vector<string> UserBackupDirectory::get_backup_filenames() const {
    lock_guard<mutex> lock(mutex_);
    vector<string> filenames;
    for (const auto& entry : bfs::directory_iterator(directory_)) {
        filenames.push_back(entry.path().filename().string());
    }
    return filenames;
}

void UserBackupDirectory::delete_file(const string& filename) {
    lock_guard<mutex> lock(mutex_);
    bfs::path backup_file = directory_ / filename;
    if (!bfs::exists(backup_file)) {
        throw FileNotFoundException(backup_file);
    }
    bool success = bfs::remove(backup_file);
    if (!success) {
        throw FailedToDeleteFileException(backup_file);
    }
}
