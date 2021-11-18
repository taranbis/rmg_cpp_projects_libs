#ifndef _TCP_CONNECTOR_HEADER_HPP
#define _TCP_CONNECTOR_HEADER_HPP 1
#pragma once

#include "tcp_handler.hpp"

class TCPConnection
{
public:
    TCPConnection(TCPConnHandler &tcpHandler, TCPConnInfo data) : 
        tcpHandler_(tcpHandler), 
        connData_(data)
    {}

    bool write(const std::string &msg)
    {
        if (tcpHandler_.writeToSocket(&connData_, msg) < 0) {
            std::cerr << "send failed";
            return false;
        }
        return true;
    }

    // std::string read() { tcpHandler_.readFromSocket(); }

private:
    TCPConnHandler &tcpHandler_;

    TCPConnInfo connData_;
};

#endif