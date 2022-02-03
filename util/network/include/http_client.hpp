#ifndef _HTTP_CLIENT_HEADER_HPP_
#define _HTTP_CLIENT_HEADER_HPP_ 1
#pragma once

#include "tcp_connection_manager.hpp"

// Usage:
// URL url = new URL("http://google.com");
// HttpURLConnection http = (HttpURLConnection)url.openConnection();
// var xhr = new XMLHttpRequest.Create(url);
// xhr.setRequestHeader("Accept", "*/*");
// var httpResponse = (HttpWebResponse)httpRequest.GetResponse();

// GET / HTTP/1.1
// Host: google.com
// Accept: */*

class HTTPClient
{
public:
    HTTPClient(TCPConnectionManager& manager) : connManager_(manager) {}

    void doRequest() {}

    void doGetRequest(const std::string& url)
    {
        //! spent alomost 1 hour figuring out why it didn't work. it was because of damn \r
        conn_->write("GET / HTTP/1.1\r\nHost: " + url + "\r\nAccept: */*\r\n\r\n");
        // conn_->write("GET / HTTP/1.1\r\nHost: www.google.com\r\nConnection: close\r\n\r\n");
    }

    // start
    void start(const std::string& ipAddress, uint16_t port = 80)
    {
        std::unique_ptr<TCPConnection> conn = connManager_.openConnection(ipAddress, port);
        if (!conn) {
            std::cerr << "could not open connection\n";
            return;
        }
        conn_ = std::move(conn);

        auto printingFunction = [](std::vector<char> buffer) {
            std::cout << "[HTTPClient] ----- Number of bytes read: " << buffer.size() << std::endl;
            std::cout << "[HTTPClient] ----- Message: " << std::endl;

            for (const auto& b : buffer) { std::cout << b; }
            std::cout << std::endl;
        };

        conn_->newBytesIncomed.connect(printingFunction);
        conn_->startReadingDataFromSocket();
    }

private:
    class HttpConnection;
    friend class HttpConnection;
    std::unique_ptr<TCPConnection> conn_;

    TCPConnectionManager& connManager_;
};

class HttpConnection : public TCPConnection
{
public:
    // HttpConnection(HTTPClient& parent) : parent_(parent){}

    static std::unique_ptr<TCPConnection> openHttpConnection()
    {
        // return parent_.
        return {};
    }

private:
    const HTTPClient parent_;
};

#endif //!_HTTP_CLIENT_HEADER_HPP_