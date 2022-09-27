#include "transfer_info.h"

#include "util/file.h"
#include "util/string_utils.h"

namespace client {

InvalidTransferInfoFile::InvalidTransferInfoFile(const bfs::path& info_file, const string& error)
    : invalid_argument{"Invalid Transfer File: " + info_file.string() + "\nError: " + error} {}

TransferInfo::TransferInfo(bip::tcp::endpoint server, string client_name, bfs::path transfer_file)
    : server_(std::move(server)),
      client_name_(std::move(client_name)),
      transfer_file_(std::move(transfer_file)) {}

const bfs::path TransferInfo::DEFAULT_FILENAME_ = bfs::path("transfer.info");

TransferInfo TransferInfo::from_file(const bfs::path& info_file) {
    util::File from_file{info_file};
    vector<string> lines{from_file.read_lines()};
    if (lines.size() != NUM_LINES_IN_INFO_FILE_) {
        throw InvalidTransferInfoFile(info_file, "Wrong number of lines");
    }

    try {
        return TransferInfo{util::string_to_endpoint(lines[0]),
                            lines[1],
                            bfs::path(lines[2])};
    } catch (const std::exception& e) {
        throw InvalidTransferInfoFile(info_file, e.what());
    }
}

bool TransferInfo::operator==(const TransferInfo& rhs) const {
    return (server_ == rhs.server_) && (client_name_ == rhs.client_name_) && (transfer_file_ == rhs.transfer_file_);
}

bool TransferInfo::operator!=(const TransferInfo& rhs) const {
    return !operator==(rhs);
}

std::ostream& operator<<(std::ostream& stream, const TransferInfo& info) {
    return stream << "TransferInfo: " << info.server_ << " : " << info.client_name_ << " : " << info.transfer_file_;
}

}  // namespace client
