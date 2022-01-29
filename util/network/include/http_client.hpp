#ifndef _HTTP_CLIENT_HEADER_HPP_
#define _HTTP_CLIENT_HEADER_HPP_ 1
#pragma once

#include "tcp_connection_manager.hpp"

//Usage: 

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

    void doGetRequest()
    {
        //! spent alomost 1 hour figuring out why it didn't work. it was because of damn \r
        conn_->write("\rGET / HTTP/1.1\n\rHost: localhost:8000\n\rAccept: */*");
    }

    // start
    void start()
    {
        conn_ = std::move(connManager_.openConnection("127.0.0.1", 8000));

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