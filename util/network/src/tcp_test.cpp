#include "tcp_connection_manager.hpp"
#include "tcp_connection.hpp"
#include "tcp_server.hpp"

/**
 * Since I am starting a server one can connect as well using
 * telnet localhost 12301 and write messages
 * or using nc localhost 12301
 */
int main()
{
    std::atomic<bool> finish{false};
    TCPConnectionManager handler;
    TCPServer server(handler);
    server.start("127.0.0.1", 12301);

    std::vector<std::shared_ptr<TCPConnection>> connections;

    auto printingFunction = [](std::vector<char> buffer) {
        std::cout << "[main] ----- Number of bytes read: " << buffer.size() << std::endl;
        std::cout << "[main] ----- Message: ";

        for (const auto& b : buffer) { std::cout << b; }
        std::cout << std::endl;
    };

    handler.newConnection.connect([&connections, &printingFunction](std::shared_ptr<TCPConnection> conn) {
        // std::unique_ptr<TCPConnection> conn = std::move(tmp);
        conn->newBytesIncomed.connect(printingFunction);
        conn->startReadingDataFromSocket();
        connections.emplace_back(conn);
    });

//----------------------------- Client Code ---------------------------------------------
    std::unique_ptr<TCPConnection> client = handler.openConnection("127.0.0.1", 12301);
    if (!client) {
        std::cerr << "socket wasn't open" << std::endl;
        return -1;
    }

    std::thread producerThread([&]() {
        static int i = 0;
        while (!finish.load(std::memory_order_relaxed)) {
            std::string msg = "msg no. " + std::to_string(i++);
            client->write(msg);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    });
//----------------------------------------------------------------------------------------

    std::thread inputThread([&] {
        if (std::cin.get() == 'n') {
            handler.finish.store(true, std::memory_order_relaxed);
            finish.store(true, std::memory_order_relaxed);
        }
    });

    // producerThread.join();
    inputThread.join();
    return 0;
}