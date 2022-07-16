#include "request_parser.h"

RequestParser::RequestParser(unique_ptr<AbstractRequestReader> reader)
    : reader_(std::move(reader)) {}

unique_ptr<ProtocolRequest> RequestParser::parse_message(ProtocolVersion expected_version) {
    uint32_t user_id{reader_->read_uint32()};
    ProtocolVersion version{reader_->read_uint8()};
    if (version != expected_version) {
        throw VersionMismatchException{expected_version, version};
    }
    RequestOP request_op = static_cast<RequestOP>(reader_->read_uint8());

    string filename;
    vector<uint8_t> payload;

    switch (request_op) {
        case RequestOP::BACKUP_FILE:
            filename = read_filename();
            payload = read_payload();
            return unique_ptr<BackupFileRequest>(new BackupFileRequest(user_id, version, filename, payload));
        case RequestOP::RESTORE_FILE:
            filename = read_filename();
            return unique_ptr<RestoreFileRequest>(new RestoreFileRequest(user_id, version, filename));
        case RequestOP::DELETE_FILE:
            filename = read_filename();
            return unique_ptr<DeleteFileRequest>(new DeleteFileRequest(user_id, version, filename));
        case RequestOP::LIST_FILES:
            return unique_ptr<ListFilesRequest>(new ListFilesRequest(user_id, version));
        default:
            throw InvalidRequestException(static_cast<uint8_t>(request_op));
    }
}

string RequestParser::read_filename() {
    uint16_t length{reader_->read_uint16()};
    vector<uint8_t> payload{reader_->read_bytes(length)};
    return string(payload.begin(), payload.end());
}

vector<uint8_t> RequestParser::read_payload() {
    uint32_t length{reader_->read_uint32()};
    return reader_->read_bytes(length);
}
