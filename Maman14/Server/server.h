#pragma once

#include <boost/asio.hpp>
#include <memory>

#include "backup_directory_manager.h"
#include "boost_connection_manager.h"
#include "protocol/common.h"
#include "request_parser.h"

namespace bfs = boost::filesystem;
using boost::asio::ip::tcp;
using std::shared_ptr;

class Server : public std::enable_shared_from_this<Server> {
public:
    // TODO: change this to C:\backsrv for windows
    static shared_ptr<Server> get_server(unsigned short port, bfs::path root_backup_directory = bfs::temp_directory_path());
    void serve_requests();
    void handleRequest(shared_ptr<BoostConnectionManager> connection, unique_ptr<ProtocolRequest> request);

    unsigned short get_port() const { return port_; };
    ProtocolVersion get_version() const { return PROTOCOL_VERSION_; };

private:
    Server(unsigned short port, bfs::path root_backup_directory);
    void backupFile(shared_ptr<BoostConnectionManager> connection, unique_ptr<BackupFileRequest> request);
    void deleteFile(shared_ptr<BoostConnectionManager> connection, unique_ptr<DeleteFileRequest> request);
    void listFiles(shared_ptr<BoostConnectionManager> connection, unique_ptr<ListFilesRequest> request);
    void restoreFile(shared_ptr<BoostConnectionManager> connection, unique_ptr<RestoreFileRequest> request);

    BackupDirectoryManager backup_directory_manager_;
    unsigned short port_;
    static const ProtocolVersion PROTOCOL_VERSION_{1};
};
