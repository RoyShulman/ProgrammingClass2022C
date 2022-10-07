#include "connection_manager.h"

using std::make_unique;

namespace client {

ConnectionManager::ConnectionManager(bip::tcp::endpoint server)
    : io_context_{make_unique<basio::io_context>()}, server_{server}, resolver_{*io_context_}, socket_{*io_context_} {
}

void ConnectionManager::connect() {
    basio::connect(socket_, resolver_.resolve(server_));
}

void ConnectionManager::write(const string& buffer) {
    basio::write(socket_, basio::buffer(buffer));
}

string ConnectionManager::read(size_t size) {
    string buffer;
    buffer.resize(size);
    basio::read(socket_, basio::buffer(buffer));
    return buffer;
}

}  // namespace client
