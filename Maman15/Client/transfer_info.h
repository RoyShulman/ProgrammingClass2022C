#pragma once

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <stdexcept>
#include <string>

namespace bip = boost::asio::ip;
namespace bfs = boost::filesystem;
using std::invalid_argument;
using std::string;

namespace client {

class InvalidTransferInfoFile : public invalid_argument {
public:
    InvalidTransferInfoFile(const bfs::path& info_file, const string& error);
};

class TransferInfo {
public:
    // This constructor is used to make unit testing easier
    TransferInfo(bip::tcp::endpoint server, string client_name, bfs::path transfer_file);
    static TransferInfo from_file(const bfs::path& info_file);

    bool operator==(const TransferInfo& rhs) const;
    bool operator!=(const TransferInfo& rhs) const;
    friend std::ostream& operator<<(std::ostream& stream, const TransferInfo& info);

private:
    static const size_t NUM_LINES_IN_INFO_FILE = 3;

    bip::tcp::endpoint server_;
    string client_name_;
    bfs::path transfer_file_;
};

}  // namespace client
