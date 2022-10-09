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

File::File(bfs::path path, bool is_binary) : path_(std::move(path)) {
    if (!bfs::exists(path_)) {
        throw FileNotFoundException{path_};
    }

    if (is_binary) {
        istream_ = std::ifstream(path_.string(), std::ios::binary);
    } else {
        istream_ = std::ifstream(path_.string());
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

string File::read() {
    size_t filesize = bfs::file_size(path_);
    string content;
    content.resize(filesize);
    istream_.read(content.data(), content.size());
    return content;
}

}  // namespace util
}  // namespace client
