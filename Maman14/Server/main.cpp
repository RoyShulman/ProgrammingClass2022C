#include <boost/exception/diagnostic_information.hpp>
#include <boost/log/trivial.hpp>
#include <boost/make_unique.hpp>
#include <exception>
#include <memory>

#include "request_parser.h"
#include "server.h"

using std::shared_ptr;
using std::unique_ptr;

int main() {
    try {
        shared_ptr<Server> server = Server::get_server(1337);
        server->serve_requests();
    } catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(fatal) << e.what();
    } catch (...) {
        BOOST_LOG_TRIVIAL(fatal) << boost::current_exception_diagnostic_information();
    }

    BOOST_LOG_TRIVIAL(info) << "bye bye";
}
