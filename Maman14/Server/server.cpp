#include "server.h"

#include <boost/exception/diagnostic_information.hpp>
#include <boost/log/trivial.hpp>
#include <boost/make_unique.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

#include "protocol/request.h"
#include "protocol/response.h"
#include "request_parser.h"
#include "request_reader.h"
#include "string_utils.h"

using boost::asio::io_service;

using std::runtime_error;
using std::shared_ptr;
using std::unique_ptr;

class InvalidRequestOpToHandleException : public runtime_error {
public:
    InvalidRequestOpToHandleException(RequestOP op)
        : runtime_error("Invalid request op to handle: " + std::to_string(static_cast<uint8_t>(op))) {}
};

static void sendServerError(shared_ptr<BoostConnectionManager> connection, ProtocolVersion version) {
    if (connection == nullptr) {
        BOOST_LOG_TRIVIAL(warning) << "sendServerError accepted nullptr - doing nothing ";
        return;
    }

    try {
        ServerErrorResponse response{version};
        connection->send(response.pack().buffer());
    } catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << "Exception sending server error: " << e.what();
    } catch (...) {
        BOOST_LOG_TRIVIAL(fatal) << "Unknown exception sending server error: " << boost::current_exception_diagnostic_information();
    }
}

static void client_session(shared_ptr<Server> server, unique_ptr<tcp::socket> client_socket) {
    string client_ip;
    shared_ptr<BoostConnectionManager> connection = nullptr;

    try {
        client_ip = client_socket->remote_endpoint().address().to_string();
        BOOST_LOG_TRIVIAL(debug) << "[Server " << server->get_port() << "] accepted new client: " << client_ip;
        connection = shared_ptr<BoostConnectionManager>(new BoostConnectionManager(std::move(client_socket)));

        RequestParser parser{boost::make_unique<RequestReader>(connection)};
        unique_ptr<ProtocolRequest> request{parser.parse_message(server->get_version())};
        server->handleRequest(connection, std::move(request));

    } catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(fatal) << "Exception during client session: " << e.what();
        sendServerError(connection, server->get_version());
    } catch (...) {
        BOOST_LOG_TRIVIAL(fatal) << "Unknown exception in client session: " << boost::current_exception_diagnostic_information();
    }

    BOOST_LOG_TRIVIAL(debug) << "[Server " << server->get_port() << "] Closing connection with: " << client_ip;
}

template <typename Derived, typename Base>
inline std::unique_ptr<Derived> dynamic_pointer_cast(std::unique_ptr<Base>&& ptr_) {
    Derived* const converted_ptr = dynamic_cast<Derived*>(ptr_.get());
    if (!converted_ptr) {
        throw std::bad_cast();
    }

    ptr_.release();
    return std::unique_ptr<Derived>(converted_ptr);
}

void Server::backupFile(shared_ptr<BoostConnectionManager> connection, unique_ptr<BackupFileRequest> request) {
    BOOST_LOG_TRIVIAL(info) << "Backing up file: " << request->get_filename() << " for user: " << request->get_user_id();
    backup_directory_manager_.backup_file_for_user_id(request->get_user_id(), request->get_filename(), request->get_payload());
    SuccessfulBackupOrDeleteResponse response{get_version(), request->get_filename()};
    connection->send(response.pack().buffer());
}

void Server::deleteFile(shared_ptr<BoostConnectionManager> connection, unique_ptr<DeleteFileRequest> request) {
    BOOST_LOG_TRIVIAL(info) << "Deleting file: " << request->get_filename() << " for user: " << request->get_user_id();
    backup_directory_manager_.delete_file_for_user(request->get_user_id(), request->get_filename());
    SuccessfulBackupOrDeleteResponse response{get_version(), request->get_filename()};
    connection->send(response.pack().buffer());
}

void Server::listFiles(shared_ptr<BoostConnectionManager> connection, unique_ptr<ListFilesRequest> request) {
    BOOST_LOG_TRIVIAL(info) << "Listing files for  " << request->get_user_id();
    try {
        vector<string> user_filenames{backup_directory_manager_.get_backup_filenames_for_user(request->get_user_id())};
        size_t filename_length = 32;
        string filename{utils::generate_random_alphanumeric(filename_length)};
        utils::Bytearray payload;
        for (const auto& f : user_filenames) {
            payload.push_string(f);
            payload.push_string("\n");
        }

        SuccessfulListFilesResponse response{get_version(), filename, payload};
        connection->send(response.pack().buffer());
    } catch (const BackupDirectoryForUserNotFound& e) {
        BOOST_LOG_TRIVIAL(error) << "Client " << request->get_user_id() << " has no backup files";
        NoBackupFilesForClientResponse response{get_version()};
        connection->send(response.pack().buffer());
    }
}

void Server::restoreFile(shared_ptr<BoostConnectionManager> connection, unique_ptr<RestoreFileRequest> request) {
    try {
        BOOST_LOG_TRIVIAL(error) << "restoring file:" << request->get_filename() << " For: " << request->get_user_id();
        vector<uint8_t> file_content{backup_directory_manager_.get_file_content_for_user(request->get_user_id(), request->get_filename())};
        utils::Bytearray payload;
        payload.push_vector(file_content);

        SuccessfulRestoreResponse response{get_version(), request->get_filename(), payload};
        connection->send(response.pack().buffer());
    } catch (const FileNotFoundException& e) {
        BOOST_LOG_TRIVIAL(error) << "Filename " << request->get_filename() << " Not found for user: " << request->get_user_id();
        FileNotFoundResponse response{get_version(), request->get_filename()};
        connection->send(response.pack().buffer());
    }
}

void Server::handleRequest(shared_ptr<BoostConnectionManager> connection, unique_ptr<ProtocolRequest> request) {
    if (connection == nullptr || request == nullptr) {
        throw std::invalid_argument("nullptr arguments to handleRequest");
    }

    RequestOP op{request->get_request_op()};
    switch (op) {
        case RequestOP::BACKUP_FILE:
            backupFile(connection, dynamic_pointer_cast<BackupFileRequest>(std::move(request)));
            break;
        case RequestOP::DELETE_FILE:
            deleteFile(connection, dynamic_pointer_cast<DeleteFileRequest>(std::move(request)));
            break;
        case RequestOP::LIST_FILES:
            listFiles(connection, dynamic_pointer_cast<ListFilesRequest>(std::move(request)));
            break;
        case RequestOP::RESTORE_FILE:
            restoreFile(connection, dynamic_pointer_cast<RestoreFileRequest>(std::move(request)));
            break;
        default:
            throw InvalidRequestOpToHandleException(request->get_request_op());
    }
}

shared_ptr<Server> Server::get_server(unsigned short port, bfs::path root_backup_directory) {
    return shared_ptr<Server>(new Server(port, std::move(root_backup_directory)));
}

Server::Server(unsigned short port, bfs::path root_backup_directory)
    : backup_directory_manager_(std::move(root_backup_directory)), port_(port) {
    BOOST_LOG_TRIVIAL(info) << "Backup directory is: " << backup_directory_manager_.get_root_backup_directory();
}

void Server::serve_requests() {
    io_service io;
    tcp::acceptor a(io, tcp::endpoint(tcp::v4(), port_));

    BOOST_LOG_TRIVIAL(info) << "Starting to serve requests";
    for (;;) {
        unique_ptr<tcp::socket> client_socket = unique_ptr<tcp::socket>(new tcp::socket(io));
        a.accept(*client_socket);
        std::thread(client_session, shared_from_this(), std::move(client_socket)).detach();
    };
}
