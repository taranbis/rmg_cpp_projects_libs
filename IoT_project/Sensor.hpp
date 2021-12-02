#ifndef SENSOR_HEADER_HPP
#define SENSOR_HEADER_HPP 1
#pragma once

#include <fstream>
#include <random>
#include <thread>

#include "Forklift.hpp"

/**
 * @class Sensor
 * @brief Implements the functionality of a sensor.
 * @param T numeric floating point type
 * @param Communicator a communicator type via which data will be sent
 * It just needs to have a method of type SendMessage(string, string)
 * in which the first string is the message and the second is the topic
 *
 * Using this type decoupling is achieved, as we are not interested here in
 * the internal functionality of how communication is achieved
 */
template <typename T, typename Communicator, bool floating_point = std::is_floating_point<T>::value>
class Sensor
{
public:
    /* _measureVariance belongs to the sensor, known from specification */
    static constexpr T measureVariance = 0.1;

    /* data update frequency */
    static constexpr T frequency = 10;

private:
    std::random_device _rd;
    std::mt19937 _mt{_rd()};
    std::normal_distribution<T> _normDist;

    const bool _outputToFile = true;
    const std::string _fileName;

    std::thread _workThread;
    std::atomic<bool> _stop{false};

    std::shared_ptr<Forklift<T>> _forkliftPtr;
    std::shared_ptr<Communicator> _comm;

public:
    Sensor(std::shared_ptr<Forklift<T>> forkliftPtr, const std::string& fileName = {"SensorData.txt"})
            : _forkliftPtr(forkliftPtr), _fileName(fileName), _normDist(0.0, measureVariance)
    {
        // TODO: I don't want to check everywhere that the pointer is not null
        // if (_obj = forklift.lock())
        // {
        //     std::cout << "Sensor on device successful!" << std::endl;
        // }
        // else
        // {
        //     std::cout << "Sensor on device unsuccessful!" << std::endl;
        // }
        static_assert(floating_point,
                      "Please use one of the floating point types: float, "
                      "double, long double");
    }

    void RegisterCommunicator(std::shared_ptr<Communicator> comm)
    {
        // TODO: maybe also switch here to weak_ptr
        this->_comm = comm;
    }

    T MeasureXPosition()
    {
        return _forkliftPtr->GetXPosition() + _normDist(_mt);
    }

    T MeasureYPosition()
    {
        return _forkliftPtr->GetYPosition() + _normDist(_mt);
    }

    void Start()
    {
        _workThread = std::thread([&] {
            if (_outputToFile) {
                /* if it already exists, delete file */
                std::remove(_fileName.c_str());
            }
            std::ofstream outputFile(_fileName);

            auto currTime = std::chrono::high_resolution_clock::now();
            T timestamp = -1;

            while (true) {
                std::this_thread::sleep_for(std::chrono::milliseconds((int)frequency));

                /* create the payload from here */
                std::string payload;
                timestamp = ((T)std::chrono::duration_cast<std::chrono::milliseconds>(
                                         std::chrono::high_resolution_clock::now() - currTime)
                                         .count()) /
                            1000;

                T x = _forkliftPtr->GetXPosition(), y = _forkliftPtr->GetYPosition();
                T x_meas = MeasureXPosition(), y_meas = MeasureYPosition(), z_meas = 0;
                T Vx = _forkliftPtr->GetXSpeed(), Vy = _forkliftPtr->GetYSpeed();

                payload += std::to_string(timestamp) + " ";
                payload += std::to_string(x_meas) + " ";
                payload += std::to_string(y_meas) + " ";
                payload += std::to_string(y_meas);

                if (_outputToFile) {
                    outputFile << "Time: " << timestamp << " ";
                    outputFile << "x: " << x << " ";
                    outputFile << "y: " << y << " ";
                    outputFile << "x_measured: " << x_meas << " ";
                    outputFile << "y_measured: " << y_meas << " ";
                    outputFile << "Vx: " << Vx << " ";
                    outputFile << "Vy: " << Vy << " ";
                    outputFile << std::endl;
                }

                /* send message straight from sensor */
                // TODO: use different topics. 1 for x, 1 for y and 1 for timestamp
                _comm->SendMessage(std::move(payload),
                                   "/localizationData/" + std::to_string(_forkliftPtr->GetTag()));

                if (_stop.load(std::memory_order_relaxed)) { break; }
            }
        });
        _workThread.detach();
    }

    void Stop()
    {
        _stop.store(true, std::memory_order_relaxed);
    }
};

#endif /* SENSOR_HEADER_HPP */