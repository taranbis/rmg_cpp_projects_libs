#include "Communicator.hpp"
#include "KalmanFilter.hpp"
#include "Sensor.hpp"

#include <Eigen/Dense>
#include <fstream>

template <std::size_t S>
using Vector = typename Eigen::Vector<double, S>;

template <std::size_t R, std::size_t C>
using Matrix = typename Eigen::Matrix<double, R, C>;

/**
 * @class DataProcessor
 * @brief Processes received data from @param Sensor using @param Filter
 * @param Filter type of filter to be applied on data
 * @param Sensor a sensor type from which data is received
 * The type of sensor which is used (e.g. Position, velocity etc.) It just needs to provide
 * two parameters as static variables as they are specific to that type of sensor: frequency
 * which is the frequency of updates, and measureVariance. They are required for the filter.
 * If we use a different type of sensor, we instantiante a different processor class
 *
 * @param numVars number of states
 * @param numMeas number of measured states
 */
template <typename T, typename Filter, typename Sensor, std::size_t numVars, std::size_t numMeas>
class DataProcessor
{
    const T _dt = Sensor::frequency / 1000;

    /* how much acc may vary between measurements */
    const T _accVariance;

    std::unique_ptr<Filter> _filterPtr;

    const bool        _outputToFile = true;
    const std::string _fileName;
    std::ofstream     outputFile;

public:
    /* or maybe put N, M, accVariance as parameters for process */
    DataProcessor(T accVariance = 0.5, const std::string& fileName = {"ProcessedData.txt"})
        : _accVariance(accVariance), _fileName(fileName), outputFile(_fileName)
    {
        const T measureVariance = Sensor::measureVariance * Sensor::measureVariance;
        Matrix<numMeas, numMeas> R{{measureVariance, 0}, {0, measureVariance}};

        /* Set initial boundary values */
        Vector<numVars> x = Vector<numVars>::Zero();

        /* Construct filter to process data from Sensor */
        _filterPtr = std::make_unique<Filter>(x, R, accVariance * accVariance); // accVariance * accVariance
    }

    void Process(std::string&& msg)
    {
        static T          previousTimestamp = 0;
        std::stringstream ss(std::move(msg));
        Vector<numMeas>   measured;

        T x = 0, y = 0, z = 0, timestamp = 0;

        if (ss >> timestamp >> x >> y >> z)
        {
            measured[0] = x;
            measured[1] = y;
        }

        _filterPtr->PredictAndUpdate(measured, previousTimestamp - timestamp);
        previousTimestamp = timestamp;

        /* Get back the speeds from the filter */
        Vector<numVars> res = _filterPtr->GetApprox();

        DEB(previousTimestamp);
        DEB(res[0]);
        DEB(res[1]);
        DEB(res[2]);
        DEB(res[3]);
        NEWLINE();

        if (_outputToFile)
        {
            // std::cout << "Real x: \n" << x << std::endl;
            // std::cout << "measured: \n" << measured << std::endl;
            outputFile << "Time: " << timestamp << " ";
            outputFile << "x_calculated: " << res[0] << " ";
            outputFile << "Vx_calculated: " << abs(res[1]) << " ";
            outputFile << "y_calculated: " << res[2] << " ";
            outputFile << "Vy_calculated: " << abs(res[3]) << " ";
            outputFile << std::endl;
        }
    }
};

template <typename T>
using DataProcessorType =
    DataProcessor<T, KalmanFilter<T, 4, 2>, Sensor<double, Communicator>, 4, 2>;

int main()
{
    uint32_t id = 3423;

    /* This should be moved in Sensor or at least the messages should be transmitted */
    Communicator comm(std::string("tcp://localhost:1883"));
    comm.SetDefaultOptions("Disconnected");

    comm.GetConnectOptions().set_automatic_reconnect(false);
    comm.Connect();
    comm.AddTopic("info");

    // comm.AddTopic(std::to_string(id) + "/localization/XCoordinate");
    // comm.AddTopic(std::to_string(id) + "/localization/YCoordinate");
    // comm.AddTopic(std::to_string(id) + "/timestamp");

    // TODO: DELETE ME
    comm.AddTopic("/localizationData/" + std::to_string(id)); 

    std::shared_ptr<DataProcessorType<double>> processor =
        std::make_shared<DataProcessorType<double>>();

    comm.ProcessMessages(processor);

    return 0;
}