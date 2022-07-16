#pragma once

#include <boost/filesystem.hpp>
#include <exception>
#include <mutex>
#include <string>
#include <vector>

namespace bfs = boost::filesystem;
using std::mutex;
using std::runtime_error;
using std::string;
using std::vector;

/**
 * @brief A base exception for all things file path related
 * 
 */
class FilePathException : public runtime_error {
public:
    FilePathException(string what, bfs::path full_path);

    const string& get_filename() const { return filename_; };
    const bfs::path& get_full_path() const { return full_path_; };

protected:
    string filename_;
    bfs::path full_path_;
};

class FileAlreadyExistsException : public FilePathException {
public:
    FileAlreadyExistsException(bfs::path full_path);
};

class FileNotFoundException : public FilePathException {
public:
    FileNotFoundException(bfs::path full_path);
};

class FailedToDeleteFileException : public FilePathException {
public:
    FailedToDeleteFileException(bfs::path full_path);
};

/**
 * @brief A class representing a single user's backup directory.
 * Allows a single user to backup, delete and restore files.
 */
class UserBackupDirectory {
public:
    UserBackupDirectory(bfs::path directory);

    void backup_file(const string& filename, const vector<uint8_t>& payload);
    const vector<uint8_t> get_backup_file_content(const string& filename) const;
    const vector<string> get_backup_filenames() const;
    void delete_file(const string& filename);

private:
    bfs::path directory_;
    // Operations on the directory should be synchronized
    mutable mutex mutex_;
};
