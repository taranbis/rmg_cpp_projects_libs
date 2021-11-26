#include "tcp_handler.hpp"
#include "tcp_connector.hpp"
#include "tcp_server.hpp"

/**
 * Since I am starting a server one can connect as well using
 * telnet localhost 12301 and write messages
 */
int main()
{
    TCPConnHandler handler;
    TCPServer      server(handler);
    server.start("127.0.0.1", 12301);

    handler.newConnection.connect([&server](TCPConnInfo data) {
        std::thread th(&TCPConnHandler::readDataFromSocket, &server.tcpHandler_, data);
        th.detach();
    });

    // There is no need to have 2 TCPHandlers
    TCPConnInfo data = handler.openConnection("127.0.0.1", 12301);
    if (data.sockfd < 0) {
        std::cerr << "socket wasn't open" << std::endl;
        return -1;
    }
    TCPConnection client(handler, data);

    std::atomic<bool> finish{false};
    std::cerr << __LINE__ << std::endl;

    std::thread producerThread([&]() {
        static int i = 0;
        while (!finish.load(std::memory_order_relaxed)) {
            std::string msg = "msg no. " + std::to_string(i++);
            client.write(msg);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    });

    std::cerr << __LINE__ << std::endl;

    std::thread inputThread([&] {
        if (std::cin.get() == 'n') {
            server.tcpHandler_.finish.store(true, std::memory_order_relaxed);
            finish.store(true, std::memory_order_relaxed);
        }
    });

    producerThread.join();
    inputThread.join();
    return 0;
}