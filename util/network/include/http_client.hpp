#ifndef _HTTP_CLIENT_HEADER_HPP_
#define _HTTP_CLIENT_HEADER_HPP_ 1
#pragma once

#include "tcp_handler.hpp"

class HTTPClient
{
private:
    TCPConnHandler& connManager;
};

#endif