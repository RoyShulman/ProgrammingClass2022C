#pragma once
#include <boost/filesystem.hpp>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace bfs = boost::filesystem;
using std::runtime_error;
using std::string;
using std::vector;

namespace client {
namespace util {

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

class FileNotFoundException : public FilePathException {
public:
    explicit FileNotFoundException(bfs::path full_path);
};

/**
 * @brief Wrapper class to work with files
 *
 */
class File {
public:
    explicit File(bfs::path path);
    vector<string> read_lines();

private:
    bfs::path path_;
    std::ifstream istream_;
};

}  // namespace util
}  // namespace client
