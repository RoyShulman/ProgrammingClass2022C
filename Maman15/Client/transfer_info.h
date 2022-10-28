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

/**
 * @brief Stores information about the file the client wants to transfer to the server,
 * as well as the server address and the client name
 * 
 */
class TransferInfo {
private:
    static const size_t NUM_LINES_IN_INFO_FILE_ = 3;
    static const bfs::path DEFAULT_FILENAME_;

    bip::tcp::endpoint server_;
    string client_name_;
    bfs::path transfer_file_;

public:
    // This constructor is used to make unit testing easier
    TransferInfo(bip::tcp::endpoint server, string client_name, bfs::path transfer_file);

    /**
     * @brief Read the information from the given file
     * 
     * @param info_file - File to read from
     * @return TransferInfo 
     */
    static TransferInfo from_file(const bfs::path& info_file = DEFAULT_FILENAME_);

    bool operator==(const TransferInfo& rhs) const;
    bool operator!=(const TransferInfo& rhs) const;
    friend std::ostream& operator<<(std::ostream& stream, const TransferInfo& info);

    const string& get_client_name() const { return client_name_; };
    const bip::tcp::endpoint& get_server() const { return server_; };
    const bfs::path& get_transfer_file() const { return transfer_file_; };
};

}  // namespace client
