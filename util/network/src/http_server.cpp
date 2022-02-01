#include "http_server.hpp"

/**
 * Since I am starting a server one can connect as well using
 * telnet localhost 12301 and write messages
 * or using nc localhost 12301
 */
int main()
{
    std::atomic<bool> finish{false};
    TCPConnectionManager handler;
    HTTPServer httpServer(handler);

    httpServer.start("127.0.0.1", 8000);

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