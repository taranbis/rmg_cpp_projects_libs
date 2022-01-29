#include "http_client.hpp"

/**
 * Since I am starting a server one can connect as well using
 * telnet localhost 12301 and write messages
 * or using nc localhost 12301
 */
int main()
{
    std::atomic<bool> finish{false};
    TCPConnectionManager handler;
    HTTPClient httpClient(handler);

    //-------------------------- Client Code ---------------------------------------------
    httpClient.start();
    httpClient.doGetRequest();

    // std::thread producerThread([&]() {
    //     static int i = 0;
    //     while (!finish.load(std::memory_order_relaxed)) {
    //         std::string msg = "msg no. " + std::to_string(i++);
    //         client->write(msg);
    //         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //     }
    // });
    //--------------------------------------------------------------------------------------

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