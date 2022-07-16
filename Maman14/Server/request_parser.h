#pragma once
#include <memory>
#include <stdexcept>
#include <string>

#include "protocol/request.h"
#include "request_reader.h"

using std::runtime_error;
using std::string;
using std::unique_ptr;

class VersionMismatchException : public runtime_error {
public:
    VersionMismatchException(ProtocolVersion expected, ProtocolVersion real)
        : runtime_error("Expected version: " + std::to_string(expected) + " Got: " + std::to_string(real)) {}
};

/**
 * @brief Class to parse incoming messages.
 * We want reader_ to be a unique_ptr so that no one else can
 * read from the buffer but us
 *
 */
class RequestParser {
public:
    RequestParser(unique_ptr<AbstractRequestReader> reader);
    unique_ptr<ProtocolRequest> parse_message(ProtocolVersion expected_version);

private:
    string read_filename();
    vector<uint8_t> read_payload();
    unique_ptr<AbstractRequestReader> reader_;
};
