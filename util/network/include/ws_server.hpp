#ifndef _WEB_SOCKET_SERVER_HEADER_HPP_
#define _WEB_SOCKET_SERVER_HEADER_HPP_ 1
#pragma once

#include "tcp_connection.hpp"

class WSConnection : TCPConnection
{
private:
};

class WSConnManager
{
	//receive a request. 
    void addConnection() {}

private:
    std::vector<WSConnection*> connections_;
};

class WebSocketService
{

private:
    WSConnManager wsMgr_;
};

#endif //! _WEB_SOCKET_SERVER_HEADER_HPP_