#ifndef _HTTP_SERVER_HEADER_HPP_
#define _HTTP_SERVER_HEADER_HPP_ 1
#pragma once

#include <chrono>
#include <ctime>

#include "tcp_connection_manager.hpp"
#include "tcp_server.hpp"

/**
 * Possible impl:
 * 		QCoreApplication app(argc, argv);
 * 		QHttpServer httpServer;
 * 		httpServer.route("/", []() {
 * 		  return "Hello world";
 * 		});
 * 		httpServer.listen(QHostAddress::Any);
 * 		return app.exec();
 */

class HTTPServer : public TCPServer
{
public:
    HTTPServer(TCPConnectionManager& mgr) : TCPServer(mgr), mgr_(mgr)
    {
        mgr_.newConnection.connect([](std::shared_ptr<TCPConnection> conn) {
            auto replyFunction = [conn](std::vector<char> buffer) {
                std::cout << "[HTTPServer] ----- Number of bytes read: " << buffer.size() << std::endl;
                std::cout << "[HTTPServer] ----- Message: ";

                for (const auto& b : buffer) { std::cout << b; }
                // std::cout << std::endl;

                const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

                conn->write("HTTP/1.1 200 OK\r\n" + std::string(std::ctime(&now)) +
                            "\r\n"
                            "Transfer-Encoding: chunked\r\nContent-Type': 'application/json\r\n"
                            "{\"message\": \"This is a JSON response\"}\r\n");
            };

            conn->newBytesIncomed.connect(replyFunction);
            conn->startReadingDataFromSocket();
        });
    }

private:
    TCPConnectionManager& mgr_;
};

#endif