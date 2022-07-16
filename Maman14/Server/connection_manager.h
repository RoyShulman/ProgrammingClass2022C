#pragma once

#include <vector>

using std::vector;

class AbstractConnectionManager {
public:
    virtual void send(const vector<uint8_t>& to_send) = 0;
    virtual vector<uint8_t> recv(size_t size) = 0;
    virtual ~AbstractConnectionManager() = default;
};
