#include "request.h"

#include <string>

ProtocolRequest::ProtocolRequest(uint32_t user_id, ProtocolVersion version, RequestOP op)
    : user_id_(user_id), version_(version), op_(op) {}

ProtocolFilenameRequest::ProtocolFilenameRequest(uint32_t user_id, ProtocolVersion version, RequestOP op, string filename)
    : ProtocolRequest(user_id, version, op), filename_(std::move(filename)) {}

ProtocolPayloadFilenameRequest::ProtocolPayloadFilenameRequest(uint32_t user_id,
                                                               ProtocolVersion version,
                                                               RequestOP op,
                                                               string filename,
                                                               vector<uint8_t> payload)
    : ProtocolFilenameRequest(user_id, version, op, std::move(filename)), payload_(std::move(payload)) {}

BackupFileRequest::BackupFileRequest(uint32_t user_id,
                                     ProtocolVersion version,
                                     string filename,
                                     vector<uint8_t> payload)
    : ProtocolPayloadFilenameRequest(user_id, version, RequestOP::BACKUP_FILE, std::move(filename), std::move(payload)) {}

RestoreFileRequest::RestoreFileRequest(uint32_t user_id, ProtocolVersion version, string filename)
    : ProtocolFilenameRequest(user_id, version, RequestOP::RESTORE_FILE, std::move(filename)) {}

DeleteFileRequest::DeleteFileRequest(uint32_t user_id, ProtocolVersion version, string filename)
    : ProtocolFilenameRequest(user_id, version, RequestOP::DELETE_FILE, std::move(filename)) {}

ListFilesRequest::ListFilesRequest(uint32_t user_id, ProtocolVersion version)
    : ProtocolRequest(user_id, version, RequestOP::LIST_FILES) {}

InvalidRequestException::InvalidRequestException(uint8_t invalid_request_op)
    : runtime_error("Invalid request op: " + std::to_string(invalid_request_op)), invalid_request_op(invalid_request_op) {}
