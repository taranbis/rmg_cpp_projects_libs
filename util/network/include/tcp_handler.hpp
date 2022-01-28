#ifndef _TCP_HANDLER_HEADER_HPP_
#define _TCP_HANDLER_HEADER_HPP_ 1
#pragma once

#include <string>
#include <thread>
#include <cstring>
#include <functional>
#include <iostream>
#include <atomic>
#include <memory>

#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#include <boost/signals2.hpp>

#include "tcp_util.hpp"
#include "tcp_connection.hpp"

// TODO: maybe create a TCPSocket class with which this can work -> I already have
class TCPConnHandler
{
public:
    boost::signals2::signal<void(std::shared_ptr<TCPConnection>)> newConnection;
    std::atomic<bool> finish{false};

private:
    std::vector<TCPConnection*> connections_;

public:
    ~TCPConnHandler()
    {
        for (TCPConnection* conn : connections_) conn->stop();
    }

    std::unique_ptr<TCPConnection> openConnection(const std::string& destAddress, uint16_t destPort)
    {
        return openConnection(destAddress, destPort, std::string(), 0);
    }

    std::unique_ptr<TCPConnection> openConnection(const std::string& destAddress, uint16_t destPort,
                                                  const std::string& sourceAddress, uint16_t sourcePort)
    {
        if (destAddress.empty()) {
            std::cerr << "Destination address not provided" << std::endl;
            return {};
        }
        // SOCK_STREAM for TCP, SOCK_DGRAM for UDP
        int sockfd = socket(destAddress.find(".") == -1 ? AF_INET6 : AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            std::cerr << "couldn't create socket" << std::endl;
            return {};
        }

        if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) {
            std::cerr << "cannot set fd non blocking " << std::endl;
            return {};
        }

        int on = !sourceAddress.empty() ? 1 : 0;
        int res = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&on, sizeof(on));
        if (res < 0) {
            std::cerr << "couldn't set option" << std::endl;
            return {};
        }

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));

        if (!sourceAddress.empty()) {
            res = makeSockAddrAndCallFunction(
                        sourceAddress, sourcePort,
                        [sockfd](struct sockaddr* addr, socklen_t size) { return bind(sockfd, addr, size); });
            if (res < 0) {
                std::cerr << "couldn't bind source address and port" << std::endl;
                return {};
            }
        }

        res = makeSockAddrAndCallFunction(destAddress, destPort, [sockfd](struct sockaddr* addr, socklen_t size) {
            return connect(sockfd, addr, size) >= 0 || errno == EINPROGRESS;
        });

        if (res < 0) {
            std::cerr << "couldn't connect to destination address and port" << std::endl;
            return {};
        }

        const TCPConnInfo connInfo{.sockfd = sockfd, .peerIP = destAddress, .peerPort = destPort};
        std::unique_ptr<TCPConnection> conn{new TCPConnection(connInfo)};
        connections_.push_back(conn.get());
        return conn;
    }

    std::unique_ptr<TCPConnection> start(const std::string& ipAddr, uint16_t port)
    {
        return openListenSocket(ipAddr, port);
    }

    bool start(int listenSocket)
    {
        // if ((new_socket = accept(listenSocket, (struct sockaddr *)&address,
        //                          (socklen_t *)&addrlen)) < 0) {
        //     perror("accept");
        //     exit(EXIT_FAILURE);
        // }
        return false;
    }

    // TODO: this should return a TCPConnection
    std::unique_ptr<TCPConnection> openListenSocket(const std::string& ipAddr, uint16_t port)
    {
        if (ipAddr.empty()) {
            std::cerr << "No peer address provided" << std::endl;
            return {};
        }

        int sockfd = socket(ipAddr.find(".") == -1 ? AF_INET6 : AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            std::cerr << "couldn't create socket" << std::endl;
            return {};
        }

        if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) {
            std::cerr << "cannot set fd non blocking " << std::endl;
            return {};
        }

        int on = 1;
        int res = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&on, sizeof(on));
        if (res < 0) {
            std::cerr << "couldn't set option" << std::endl;
            close(sockfd);
            return {};
        }

        res = makeSockAddrAndCallFunction(ipAddr, port, [sockfd](struct sockaddr* addr, socklen_t size) {
            return bind(sockfd, addr, size);
        });

        if (res < 0) {
            // couldn't bind source address and port;
            std::cerr << "cannot bind to " << ipAddr << ":" << port << " (" << errno << ", " << strerror(errno)
                      << ")" << std::endl;
            close(sockfd);
            return {};
        }

        // the second number represents the maximum number of accepted connections, before they start being refused
        if (listen(sockfd, 1024) < 0) {
            close(sockfd);
            return {};
        }

        const TCPConnInfo connInfo{.sockfd = sockfd, .peerIP = ipAddr, .peerPort = port};
        std::unique_ptr<TCPConnection> conn{new TCPConnection(connInfo)};
        std::thread th(&TCPConnHandler::checkForConnections, this, conn.get());
        th.detach();
        connections_.push_back(conn.get());
        return conn;
    }

    void checkForConnections(TCPConnection* conn)
    {
        const TCPConnInfo data = conn->connData_;
        fd_set set;
        int newSockFd;
        int sockfd = conn->connData_.sockfd;

        while (!finish.load(std::memory_order_relaxed)) {
            FD_ZERO(&set);        /* clear the set */
            FD_SET(sockfd, &set); /* add the socket file descriptor to the set */
            int activity = select(sockfd + 1, &set, NULL, NULL, NULL);

            if ((activity < 0) && (errno != EINTR)) { std::cerr << "select error " << std::endl; }

            // std::cerr << "activity = " << activity << std::endl;

            if (activity == 0) continue;
            // If something happened on the socket, then its an incoming connection
            if (FD_ISSET(sockfd, &set)) {
                newSockFd = makeSockAddrAndCallFunction(data.peerIP, data.peerPort,
                                                        [sockfd](struct sockaddr* addr, socklen_t size) {
                                                            return accept(sockfd, addr, &size);
                                                        });
                if (newSockFd < 0) {
                    std::cerr << "accept error" << std::endl;
                    continue;
                }
                std::cerr << "New Connection, socket fd is " << newSockFd << ", destIP is " << data.peerIP
                          << ", port : " << data.peerPort << std::endl;

                const char* message = "Welcome message \r\n";
                // send new connection greeting message
                if (send(newSockFd, message, strlen(message), 0) != strlen(message)) { perror("send"); }

                std::cerr << "Welcome message sent successfully " << std::endl;

                std::shared_ptr<TCPConnection> newConn{new TCPConnection(data)};
                newConn->connData_.sockfd = newSockFd;
                connections_.push_back(newConn.get());
                newConnection(newConn);
            }
        }
    }
};

#endif //!_TCP_HANDLER_HEADER_HPP_