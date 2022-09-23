#include "file.h"

#include <iostream>

namespace client {
namespace util {

FilePathException::FilePathException(string what, bfs::path full_path)
    : runtime_error(std::move(what)),
      filename_(full_path.filename().string()),
      full_path_(std::move(full_path)) {}

FileNotFoundException::FileNotFoundException(bfs::path full_path)
    : FilePathException("Filename: " + full_path.string() + " not found", std::move(full_path)) {}

File::File(bfs::path path) : path_(std::move(path)), istream_(path_.string()) {
    if (!bfs::exists(path_)) {
        throw FileNotFoundException{path_};
    }
}

vector<string> File::read_lines() {
    string line;
    vector<string> lines;
    while (getline(istream_, line)) {
        lines.push_back(line);
    }

    return lines;
}

}  // namespace util
}  // namespace client
