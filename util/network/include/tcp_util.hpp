#ifndef _TCP_UTIL_HEADER_HPP_
#define _TCP_UTIL_HEADER_HPP_ 1
#pragma once

#include <arpa/inet.h>

inline int makeSockAddrAndCallFunction(const std::string& ipAddr, uint16_t port,
                                       std::function<int(struct sockaddr*, socklen_t)> func)
{
    if (ipAddr.find(".") == -1) {
        struct sockaddr_in6 addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin6_family = AF_INET6;
        if (inet_pton(AF_INET6, ipAddr.c_str(), &addr.sin6_addr) == 0) { return -1; }
        addr.sin6_port = htons(port);

        return func((struct sockaddr*)&addr, sizeof(addr));
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, ipAddr.c_str(), &addr.sin_addr) == 0) { return -1; }
    addr.sin_port = htons(port);

    return func((struct sockaddr*)&addr, sizeof(addr));
}

#endif