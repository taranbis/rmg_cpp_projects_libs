#include "Communicator.hpp"
#include "Forklift.hpp"
#include "Sensor.hpp"

#include <memory>
#include <random>

// TODO: select different types of maps: eg anywhere in a square, lines (like a grid)
/**
 * @class Warehouse
 * @brief Implements the functionality of a sensor.
 * @param T numeric floating point type
 * @param Communicator a communicator type via which data will be sent
 * It just needs to have a method of type SendMessage(string, string)
 * in which the first string is the message and the second is the topic
 *
 * Using this type decoupling is achieved, as we are not interested here in
 * the internal functionality of how communication is achieved
 */
class Warehouse
{
public:
    std::random_device _rd;
    std::mt19937       _mt{_rd()};

    /* After the travel time, the forklift stops for a duration between 5 to 10s */
    std::uniform_int_distribution<int> _stopTime;

    /* It can travel in a square of 150 x 150 */
    std::uniform_real_distribution<double> _positions;

    std::atomic<bool> stop{false};

    Warehouse() : _stopTime(2, 5), _positions(0, 300)
    {
    }

    /**
     * The warehouse can operate different automous devices: Forklifts, Cranes etc.
     * Once it receives a device to use, it starts moving it
     * All the devices need is two methods: MoveToPosition(double, double) and Wait(int)
     */
    template <typename Device>
    void UseDevice(std::weak_ptr<Device> devicePtr)
    {
        /* We want to make sure the device is still available */
        std::shared_ptr<Device> device;
        if (device = devicePtr.lock())
        {
            std::cout << "Device availble" << std::endl;
        }
        else
        {
            std::cout << "Device not available anymore!" << std::endl;
        }

        while (!stop.load(std::memory_order_relaxed))
        {
            double xPosition = _positions(_mt);
            double yPosition = _positions(_mt);
            int    stopTime  = _stopTime(_mt);

            DEB(xPosition);
            DEB(yPosition);
            DEB(stopTime);

            device->MoveToPosition(xPosition, yPosition);

            /* Here happens the lifting */
            device->Wait(_stopTime(_mt));
        }
    }

    void ChangeMap()
    {
    }

    void EndWork()
    {
        stop.store(true, std::memory_order_relaxed);
    }
};

// TODO: plot to see that data looks nice (Python)
// TODO: add the parallel_hash_map somehow
// TODO: add all sorts of functonalities, be creative
// maybe use Andrei Alexandrescu's stuff to create object on scope and destroy on exit

// use topics smart
// Assessed: code structure, creativity, most important -> how I structure code
// use libraries for anything
int main()
{
    uint32_t                          id = 3423;
    std::shared_ptr<Forklift<double>> forklift = std::make_shared<Forklift<double>>(id);

    std::shared_ptr<Communicator> comm =
        std::make_shared<Communicator>("tcp://localhost:1883");

    comm->SetDefaultOptions("Sensor Disconnected");
    comm->Connect();

    // comm->AddTopic(std::to_string(id) + "/localization/XCoordinate");
    // comm->AddTopic(std::to_string(id) + "/localization/YCoordinate");
    // comm->AddTopic(std::to_string(id) + "/timestamp");

    comm->AddTopic("/localizationData/" + std::to_string(id));

    Sensor<double, Communicator> positionSensor(forklift);
    positionSensor.RegisterCommunicator(comm);
    positionSensor.Start();

    /* Assign the forklift to a warehouse to move it around */
    Warehouse warehouse;

    std::thread inputThread([&] {
        if (std::cin.get() == 'n')
        {
            warehouse.EndWork();
            positionSensor.Stop();
            comm->Disconnect("Sensor Disconnected");
        }
    });

    warehouse.UseDevice<Forklift<double>>(forklift);
    inputThread.join();

    return 0;
}