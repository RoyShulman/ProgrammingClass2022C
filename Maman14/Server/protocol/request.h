#pragma once
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "common.h"

using std::runtime_error;
using std::string;
using std::unique_ptr;
using std::vector;

enum class RequestOP : uint8_t {
    BACKUP_FILE = 100,

    RESTORE_FILE = 200,
    DELETE_FILE = 201,
    LIST_FILES = 202,
};

class ProtocolRequest {
protected:
    ProtocolRequest(uint32_t user_id, ProtocolVersion version, RequestOP op);

    uint32_t user_id_;
    ProtocolVersion version_;
    RequestOP op_;

public:
    virtual ~ProtocolRequest() = default;
    uint32_t get_user_id() const { return user_id_; };
    RequestOP get_request_op() const { return op_; };
};

class ProtocolFilenameRequest : public ProtocolRequest {
protected:
    ProtocolFilenameRequest(uint32_t user_id, ProtocolVersion version, RequestOP op, string filename);

    string filename_;

public:
    const string& get_filename() const { return filename_; };
};

class ProtocolPayloadFilenameRequest : public ProtocolFilenameRequest {
protected:
    ProtocolPayloadFilenameRequest(uint32_t user_id,
                                   ProtocolVersion version,
                                   RequestOP op,
                                   string filename,
                                   vector<uint8_t> payload);

    vector<uint8_t> payload_;

public:
    const vector<uint8_t>& get_payload() const { return payload_; };
};

class BackupFileRequest : public ProtocolPayloadFilenameRequest {
public:
    BackupFileRequest(uint32_t user_id,
                      ProtocolVersion version,
                      string filename,
                      vector<uint8_t> payload);
};

class RestoreFileRequest : public ProtocolFilenameRequest {
public:
    RestoreFileRequest(uint32_t user_id,
                       ProtocolVersion version,
                       string filename);
};

class DeleteFileRequest : public ProtocolFilenameRequest {
public:
    DeleteFileRequest(uint32_t user_id,
                      ProtocolVersion version,
                      string filename);
};

class ListFilesRequest : public ProtocolRequest {
public:
    ListFilesRequest(uint32_t user_id, ProtocolVersion version);
};

class InvalidRequestException : public runtime_error {
public:
    InvalidRequestException(uint8_t invalid_request_op);

    uint8_t invalid_request_op;
};
