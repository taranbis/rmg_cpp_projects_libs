#include "KalmanFilter.hpp"

#include <fstream>
#include <random>

template <std::size_t S>
using Vector = typename Eigen::Vector<double, S>;

template <std::size_t R, std::size_t C>
using Matrix = typename Eigen::Matrix<double, R, C>;

int main()
{
    double                dt = 0.1;
    constexpr std::size_t N = 4, M = 2;

    std::random_device               rd;
    std::mt19937                     mt(rd());
    double                           measureVariance = 0.1;
    double                           accVariance = 0.1; // how much acc may vary between measurements
    std::normal_distribution<double> normDist(0.0, measureVariance);

    // TODO: pass in an initial v and an initial x for x state variable
    measureVariance = measureVariance * measureVariance;
    Matrix<M, M>   R{{measureVariance, 0}, {0, measureVariance}};
    Kalman<double> kalmanFilter(dt, R, accVariance * accVariance);

    Vector<M> measured = Vector<M>::Zero();
    /* Set initial values */
    Vector<N> x = Vector<N>::Zero();
    x[0] = 0.0;
    x[1] = 1.0;
    x[2] = 0.0;
    x[3] = 1.0;

    std::ofstream outputFile("KalmanData.txt");

    for (int i = 0; i < 1000; i++) {
        x[0] = x[0] + dt * x[1];
        x[2] = x[2] + dt * x[3];
        if (i > 500) {
            x[1] *= 0.9;
            x[3] *= 0.9;
        }

        // measured value together with measured noise
        measured[0] = x[0] + normDist(mt);
        measured[1] = x[2] + normDist(mt);

        std::cout << "Real x: \n" << x << std::endl;
        std::cout << "measured: \n" << measured << std::endl;

        kalmanFilter.PredictAndUpdate(measured, dt);

        outputFile << "Iteration: " << i << std::endl;
        outputFile << "Real results: " << std::endl;
        outputFile << x[0] << " " << x[1] << " " << x[2] << " " << x[3] << std::endl;
        Vector<N> res = kalmanFilter.GetApprox();
        outputFile << "Filtered results: " << std::endl;
        outputFile << res[0] << " " << res[1] << " " << res[2] << " " << res[3] << std::endl;
    }

    return 0;
}