#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "../bytearray.h"
#include "common.h"

using std::string;
using std::vector;

enum class ResponseOP : uint16_t {
    SUCCESSFUL_RESTORE = 210,
    SUCCESSFUL_LIST_FILES = 211,
    SUCCESSFUL_BACKUP_OR_DELETE = 212,

    FILE_NOT_FOUND = 1001,
    NO_BACKUP_FILES_FOR_CLIENT = 1002,
    SERVER_ERROR = 1003,
};

/**
 * @brief The base class for all protocol responses.
 * The constructor the protected since we don't want to be able to
 * construct this class. Messages that are specified in the protocol
 * should inherit from this class and declare their constructor public
 *
 */
class ProtocolResponse {
protected:
    ProtocolResponse(ResponseOP op, ProtocolVersion version);

    utils::Bytearray pack_header() const;

    ResponseOP op_;
    ProtocolVersion version_;

public:
    virtual ~ProtocolResponse() = default;
    virtual utils::Bytearray pack() const;
};

/**
 * @brief Base class for all protocol responses that send a filename
 *
 */
class FilenameProtocolResponse : public ProtocolResponse {
protected:
    FilenameProtocolResponse(ResponseOP op, ProtocolVersion version, string filename);

    utils::Bytearray pack_header_filename(const string& filename) const;

    string filename_;

public:
    virtual utils::Bytearray pack() const override;
};

/**
 * @brief Base class for all protocol responses that send pack payload and filename
 *
 */
class PayloadFilenameProtocolResponse : public FilenameProtocolResponse {
protected:
    PayloadFilenameProtocolResponse(ResponseOP op, ProtocolVersion version, string filename, utils::Bytearray payload);

    utils::Bytearray pack_header_filename_payload(const string& filename, const utils::Bytearray& payload) const;

    utils::Bytearray payload_;

public:
    virtual utils::Bytearray pack() const override;
};

class SuccessfulRestoreResponse : public PayloadFilenameProtocolResponse {
public:
    SuccessfulRestoreResponse(ProtocolVersion version, string filename, utils::Bytearray payload);
};

class SuccessfulListFilesResponse : public PayloadFilenameProtocolResponse {
public:
    SuccessfulListFilesResponse(ProtocolVersion version, string filename, utils::Bytearray payload);
};

class SuccessfulBackupOrDeleteResponse : public FilenameProtocolResponse {
public:
    SuccessfulBackupOrDeleteResponse(ProtocolVersion version, string filename);
};

class FileNotFoundResponse : public FilenameProtocolResponse {
public:
    FileNotFoundResponse(ProtocolVersion version, string filename);
};

class NoBackupFilesForClientResponse : public ProtocolResponse {
public:
    NoBackupFilesForClientResponse(ProtocolVersion version);
};

class ServerErrorResponse : public ProtocolResponse {
public:
    ServerErrorResponse(ProtocolVersion version);
};
