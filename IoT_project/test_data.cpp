#include <chrono>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <thread>

#include "util.hpp"

typedef struct Forklift {
    // uint64_t timestamp;
    uint32_t id;
    float x, y, z;

    Forklift(uint32_t tag) : id(tag) {}

    ~Forklift() = default;

    Forklift(const Forklift& other) = delete;
    Forklift& operator=(const Forklift& other) = delete;

    Forklift(Forklift&& other) = default;
    Forklift& operator=(Forklift&& other) = default;
} Forklift;

void publish(std::string payload);

int main()
{
    std::random_device device;
    std::mt19937 gen{device()};
    // 0.3 is composed such that when composing speed per x and y it will lead to a maximum of 15 km/h
    std::uniform_real_distribution<float> disXY(0.0, 0.3);
    std::uniform_real_distribution<float> disZ(0.0, 0.05);
    std::uniform_int_distribution<uint32_t> idDis(0, std::numeric_limits<uint32_t>::max());

    // For a duration between 15s to 60s the forklift will travel
    // After the travel time it will stop for a duration between 5 to 15s
    std::uniform_int_distribution<uint32_t> travelTimeDis(15, 60);
    std::uniform_int_distribution<uint32_t> stopTimeDis(5, 15);

    // After reading a little about forklifts, we will consider the speed
    // somewhere from between 0 and 15 km/h, which is from 0 m/s to 4.16 m/s
    // When there's z speed there is no x,y speed, as it leaves or takes cargo
    // When there is x,y speed there is no z speed; makes no sense;
    // lift speed is around 0.5 m/s
    Forklift forkLift(idDis(gen));

    const auto kPeriod = std::chrono::milliseconds(100);
    auto start = std::chrono::high_resolution_clock::now();
    bool moving = true;
    uint32_t travelTime = travelTimeDis(gen) * 1000;
    uint32_t stopTime = stopTimeDis(gen) * 1000;

    while (true) {
        // end         = std::chrono::high_resolution_clock::now();
        // timeCounter = end - start;
        auto timePassed = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
                                      std::chrono::high_resolution_clock::now() - start)
                                      .count();

        if (moving && timePassed > travelTime) { // the forklift is moving => x,y vary
            stopTime = stopTimeDis(gen) * 1000;
            start = std::chrono::high_resolution_clock::now();
            moving = false;
        }

        if (!moving && timePassed > stopTime) { // the forklift is moving => z varies
            travelTime = travelTimeDis(gen) * 1000;
            start = std::chrono::high_resolution_clock::now();
            moving = true;
        }

        if (moving) {
            DEB(timePassed);
            DEB(travelTime);
            forkLift.x += disXY(gen);
            forkLift.y += disXY(gen);
            forkLift.z = 0;
        }

        if (!moving) {
            DEB(timePassed);
            DEB(stopTime);
            if (timePassed < stopTime / 2) {
                forkLift.z += disZ(gen);
            } else {
                forkLift.z -= disZ(gen);
                if (forkLift.z < 0) forkLift.z = 0;
            }
        }

        // send update values every 100 milliseconds
        std::this_thread::sleep_for(kPeriod);
        std::string payload;
        auto timestamp = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
                                     std::chrono::system_clock::now().time_since_epoch())
                                     .count();
        payload += std::to_string(timestamp) + " ";
        payload += std::to_string(forkLift.id) + " ";
        payload += std::to_string(forkLift.x) + " ";
        payload += std::to_string(forkLift.y) + " ";
        payload += std::to_string(forkLift.z);

        publish(std::move(payload));
    }
    return 0;
}

typedef struct PreviousData {
    uint64_t timestamp = -1;
    uint32_t id = -1;
    float x = 0.0f, y = 0.0f, z = 0.0f;
} PreviousData;

void publish(std::string msg)
{
    static PreviousData previousData;
    static float totalDistance;
    std::stringstream ss(std::move(msg));
    uint64_t timestamp = -1;
    uint32_t id = -1;
    float x = 0.0f, y = 0.0f, z = 0.0f;

    if (ss >> timestamp >> id >> x >> y >> z) {
        // In case of multiple forklifts, we would also check and
        // compute based on id

        uint64_t time = timestamp - previousData.timestamp;
        float distance = (x - previousData.x) * (x - previousData.x) + (y - previousData.y) * (y - previousData.y);
        float xySpeed = sqrt(distance) * 1000 / time;
        std::cout << "Speed is: " << xySpeed << "[m/s], which is " << xySpeed * 3.6 << "[km/h]" << std::endl;
        float zSpeed = sqrt(z - previousData.z) * 1000 / time;
        std::cout << "Lifting speed is: " << zSpeed << "[m/s]" << std::endl;
        totalDistance += distance;
        std::cout << "Total distance travelled by the forklift: " << totalDistance << "[m]" << std::endl;

        previousData.timestamp = timestamp;
        previousData.id = id;
        previousData.x = x;
        previousData.y = y;
        previousData.z = z;
    }
}
