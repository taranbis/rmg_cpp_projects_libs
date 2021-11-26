#include <cmath>
#include <iostream>
#include <sstream>
#include <thread>

#include "mqtt/async_client.h"
#include "mqtt/topic.h"

typedef struct PreviousData {
    uint64_t timestamp = 0;
    uint32_t id = 0;
    float    x = 0, y = 0, z = 0;
} PreviousData;

int main()
{
    // 1883 port used in the other app is the default port
    const std::string kBrokerAddress("localhost");
    const std::string kTopicName("data/localization");
    const std::string kClientID{"procressor_1"};
    const int         kQoS = 1;
    const bool        kRetainData = false;

    auto lwt = mqtt::make_message(kTopicName, "Disconnected", kQoS, kRetainData);

    mqtt::connect_options connOpts;
    connOpts.set_mqtt_version(MQTTVERSION_5);
    connOpts.set_keep_alive_interval(20);
    connOpts.set_clean_start(true);
    connOpts.set_will_message(lwt);
    connOpts.set_automatic_reconnect(true);

    mqtt::async_client client(kBrokerAddress, kClientID);

    // This initializes the client to receive messages through
    // a queue that can be read synchronously
    client.start_consuming();

    client.set_connected_handler([](const std::string&) { std::cout << "Connected to broker" << std::endl; });

    // Use the callback whenever a message is received to parse localization data
    client.set_message_callback([](mqtt::const_message_ptr msg) {
        // localization data from previous message
        static PreviousData previousData;
        static float        totalDistance = 0;

        // Reset all values if the generator disconnected
        if (msg->get_payload_str() == "Device has disconnected") {
            totalDistance = 0;
            previousData.timestamp = 0;
            previousData.x = 0;
            previousData.y = 0;
            previousData.z = 0;
            return;
        }

        std::stringstream ss(std::move(msg->get_payload_str()));
        uint64_t          timestamp = 0;
        uint32_t          id = 0;
        float             x = 0, y = 0, z = 0;

        if (ss >> timestamp >> id >> x >> y >> z) {
            // First message will not be considered for computation
            if (previousData.timestamp == 0ull) {
                previousData.timestamp = timestamp;
                previousData.x = x;
                previousData.y = y;
                previousData.z = z;
                return;
            }

            // In case of multiple forklifts, we would also check and
            // compute based on id and then dispatch based on a hash table

            // timestamps are in milliseconds
            uint64_t time = timestamp - previousData.timestamp;
            float    distance =
                        (x - previousData.x) * (x - previousData.x) + (y - previousData.y) * (y - previousData.y);
            float xySpeed = sqrt(distance) * 1000 / time;

            std::cout << "Speed is: " << xySpeed << "[m/s], which is " << xySpeed * 3.6 << "[km/h]" << std::endl;
            float zSpeed = sqrt((z - previousData.z) * (z - previousData.z)) * 1000 / time;
            std::cout << "Lifting speed is: " << zSpeed << "[m/s]" << std::endl;
            totalDistance += distance;
            std::cout << "Total distance travelled by the forklift: " << totalDistance << "[m]" << std::endl;

            previousData.timestamp = timestamp;
            previousData.id = id;
            previousData.x = x;
            previousData.y = y;
            previousData.z = z;
        }
    });

    try {
        auto tok = client.connect(connOpts);

        // get_connect_response() will also block until the result is available
        auto connResponse = tok->get_connect_response();

        // Subscribe to topic
        tok = client.subscribe(kTopicName, kQoS);
        tok->wait();

        while (true) {
            // We subscribed to only one topic, no need to check the topic of the messages received
            auto msg = client.consume_message();

            if (!msg) {
                std::this_thread::sleep_for(std::chrono::seconds(15));
                client.reconnect()->wait();
                if (client.is_connected()) {
                    client.subscribe(kTopicName, kQoS)->wait();
                    std::cout << "Connected" << std::endl;
                    continue;
                } else {
                    std::cout << "Disconnected" << std::endl;
                    break;
                }
            }
        }

        client.disconnect()->wait();
        std::cout << "Disconnected" << std::endl;
    } catch (const mqtt::exception& ex) {
        std::cerr << "MQTT Error: " << ex.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Standard Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Some error" << std::endl;
        return 1;
    }
    return 0;
}