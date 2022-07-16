#include "response.h"

ProtocolResponse::ProtocolResponse(ResponseOP op, ProtocolVersion version)
    : op_(op), version_(version) {}

utils::Bytearray ProtocolResponse::pack_header() const {
    // We assume we are running on a little endian machine
    utils::Bytearray packed_header;
    packed_header.push_u8(version_);
    packed_header.push_u16(static_cast<uint16_t>(op_));

    return packed_header;  // No copy because of RVO so it's ok to return like this
}

utils::Bytearray ProtocolResponse::pack() const {
    return pack_header();
}

FilenameProtocolResponse::FilenameProtocolResponse(ResponseOP op, ProtocolVersion version, string filename)
    : ProtocolResponse(op, version), filename_(std::move(filename)) {}

utils::Bytearray FilenameProtocolResponse::pack_header_filename(const string& filename) const {
    utils::Bytearray packed_header = pack_header();
    packed_header.push_u16(static_cast<uint16_t>(filename.length()));
    packed_header.push_string(filename);
    return packed_header;
}

utils::Bytearray FilenameProtocolResponse::pack() const {
    return pack_header_filename(filename_);
}

PayloadFilenameProtocolResponse::PayloadFilenameProtocolResponse(ResponseOP op, ProtocolVersion version, string filename, utils::Bytearray payload)
    : FilenameProtocolResponse(op, version, std::move(filename)), payload_(std::move(payload)) {}

utils::Bytearray PayloadFilenameProtocolResponse::pack_header_filename_payload(const string& filename, const utils::Bytearray& payload) const {
    utils::Bytearray packed_header = pack_header_filename(filename);
    packed_header.push_u32(payload.len());
    packed_header.push_bytes(payload);
    return packed_header;
}

utils::Bytearray PayloadFilenameProtocolResponse::pack() const {
    return pack_header_filename_payload(filename_, payload_);
}

SuccessfulRestoreResponse::SuccessfulRestoreResponse(ProtocolVersion version,
                                                     string filename,
                                                     utils::Bytearray payload)
    : PayloadFilenameProtocolResponse(ResponseOP::SUCCESSFUL_RESTORE, version, std::move(filename), std::move(payload)) {}

SuccessfulListFilesResponse::SuccessfulListFilesResponse(ProtocolVersion version,
                                                         string filename,
                                                         utils::Bytearray payload)
    : PayloadFilenameProtocolResponse(ResponseOP::SUCCESSFUL_LIST_FILES, version, std::move(filename), std::move(payload)) {}

SuccessfulBackupOrDeleteResponse::SuccessfulBackupOrDeleteResponse(ProtocolVersion version,
                                                                   string filename)
    : FilenameProtocolResponse(ResponseOP::SUCCESSFUL_BACKUP_OR_DELETE, version, std::move(filename)) {}

FileNotFoundResponse::FileNotFoundResponse(ProtocolVersion version,
                                           string filename)
    : FilenameProtocolResponse(ResponseOP::FILE_NOT_FOUND, version, std::move(filename)) {}

NoBackupFilesForClientResponse::NoBackupFilesForClientResponse(ProtocolVersion version)
    : ProtocolResponse(ResponseOP::NO_BACKUP_FILES_FOR_CLIENT, version) {}

ServerErrorResponse::ServerErrorResponse(ProtocolVersion version)
    : ProtocolResponse(ResponseOP::SERVER_ERROR, version) {}
