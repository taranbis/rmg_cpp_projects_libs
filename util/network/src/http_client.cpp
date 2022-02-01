#include "http_client.hpp"

using namespace std;

int main()
{
    std::atomic<bool> finish{false};
    TCPConnectionManager handler;
    HTTPClient httpClient(handler);

    int ipv = 0;
    const std::string url = "www.google.com";
    const std::string ipAddress = handler.dnsLookup(url, ipv = 4);
    std::cout << "ipv4 address is: " << ipAddress << std::endl;

    httpClient.start(ipAddress, 80);
    httpClient.doGetRequest(url);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    httpClient.doGetRequest(url);

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