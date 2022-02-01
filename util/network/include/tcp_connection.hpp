#ifndef _TCP_CONNECTION_HEADER_HPP_
#define _TCP_CONNECTION_HEADER_HPP_ 1
#pragma once

class TCPConnectionManager;

struct TCPKeepAliveInfo {
};

struct TCPConnInfo {
    int         sockfd = -1;
    std::string peerIP;
    uint16_t    peerPort;
};

class TCPConnection
{
public:
    TCPConnection(TCPConnInfo data) : connData_(data) {}
    TCPConnection(const TCPConnection& other) = delete;

    void stop()
    {
        finish.store(true, std::memory_order_relaxed);
        close(connData_.sockfd);
    }

    bool write(const std::string& msg)
    {
        if (send(connData_.sockfd, msg.c_str(), strlen(msg.c_str()), 0) < 0) {
            std::perror("send failed");
            return false;
        }
        return true;
    }

    void startReadingDataFromSocket()
    {
        std::thread th(&TCPConnection::readDataFromSocket, this);
        th.detach();
    }

    //TODO: read data from socket and to socket should be here not in TCPConnectionManager
    void readDataFromSocket()
    {
        std::unique_ptr<char> buffer(new char[1024]);
        while (!finish.load(std::memory_order_relaxed)) {
            int valread = read(connData_.sockfd, (void*)buffer.get(), 1024);
            if (valread != -1) {
                if (valread == 0) {
                    stop();
                    return;
                }
                if (printReceivedData) {
                    std::cout << "Number of bytes read: " << valread << std::endl;
                    std::cout << "Message: " << std::endl;

                    for (int i = 0; i < valread; ++i) std::cout << *(buffer.get() + i);
                    std::cout << std::endl;
                }

                std::vector<char> rv;
                for (int i = 0; i < valread; ++i) { rv.emplace_back(*(buffer.get() + i)); }
                newBytesIncomed(rv);
            }
        }
    }
//TODO: bug if socket is closed from the other side reading continues with nothing
public:
    // boost::signals2::signal<void(std::shared_ptr<char>)> newBytesIncomed;
    boost::signals2::signal<void(std::vector<char> )> newBytesIncomed;

private:
    std::atomic<bool> finish{false};
    TCPConnInfo connData_;
    bool printReceivedData {false};

    friend class TCPConnectionManager;
};

#endif //!_TCP_CONNECTION_HEADER_HPP_