#include "mqtt/async_client.h"
#include "mqtt/topic.h"

#include <chrono>
#include <iostream>
#include <random>
#include <thread>

int main()
{
    Forklift forklift(idDis(gen));

    // turn to milliseconds
    uint32_t travelTime = travelTimeDis(gen) * 1000;
    uint32_t stopTime   = stopTimeDis(gen) * 1000;

    while (true)
    {
        auto timePassed = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
                              std::chrono::high_resolution_clock::now() - start)
                              .count();

        if (moving && timePassed > travelTime)
        { // the forklift stops
            stopTime = stopTimeDis(gen) * 1000;
            start    = std::chrono::high_resolution_clock::now();
            moving   = false;
            continue;
        }

        if (!moving && timePassed > stopTime)
        { // the forklift is moving again
            travelTime = travelTimeDis(gen) * 1000;
            start      = std::chrono::high_resolution_clock::now();
            moving     = true;
            continue;
        }

        // Update z speed while stopped and x,y while moving
        if (!moving)
        {
            std::cout << "timePassed = " << timePassed << std::endl;
            std::cout << "stopTime = " << stopTime << std::endl;
            if (timePassed < stopTime / 2)
            {
                // half of the stopped time it lifts
                forklift.z += disZ(gen);
            }
            else
            {
                // the other half of the stopped time it descends so that it
                // it does not travel with cargo lifted
                forklift.z -= disZ(gen);
                if (forklift.z < 0)
                    forklift.z = 0;
            }
        }
        else
        {
            std::cout << "timePassed = " << timePassed << std::endl;
            std::cout << "travelTime = " << travelTime << std::endl;
            forklift.x += disXY(gen);
            forklift.y += disXY(gen);
            forklift.z = 0;
        }

        // send update values every 100 milliseconds



        return 0;
    }
