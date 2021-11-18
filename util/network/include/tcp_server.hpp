#ifndef _TCP_SERVER_HEADER_HPP_
#define _TCP_SERVER_HEADER_HPP_ 1
#pragma once

#include "tcp_handler.hpp"

class TCPServer
{
public:
    TCPConnHandler &tcpHandler_;
    TCPConnInfo data_;

public:
    TCPServer(TCPConnHandler &tcpHandler) : tcpHandler_(tcpHandler) {}
    void start(const std::string &sourceAddress, uint16_t sourcePort)
    {
        // return startListen(sourceAddress, sourcePort);
        data_ = tcpHandler_.openListenSocket(sourceAddress, sourcePort);

        std::thread th(&TCPConnHandler::checkForConnections, &tcpHandler_, data_);
        th.detach();
    }
};

#endif