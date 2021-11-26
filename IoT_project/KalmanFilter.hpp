#ifndef KALMAN_FILTER_HEADER_HPP
#define KALMAN_FILTER_HEADER_HPP 1
#pragma once

#include <iostream>

#include <Eigen/Dense>

/**
 * @class Kalman
 * @brief Implements a Kalman Filter
 * @param T The numeric floating point type
 * @param N The number of states
 * @param M The number of measured states
 */
template <typename T, std::size_t N = 4, std::size_t M = 2, bool floating_point = std::is_floating_point<T>::value>
class KalmanFilter
{
public:
    template <std::size_t S>
    using Vector = typename Eigen::Vector<T, S>;

    template <std::size_t R, std::size_t C>
    using Matrix = typename Eigen::Matrix<T, R, C>;

    /* State Vector */
    Vector<N> _x;

    /* State Transition Matrix */
    Matrix<N, N> _F;

    /* State covariance Matrix */
    Matrix<N, N> _S;

    /* Measurement Noise Covariance Matrix */
    Matrix<M, M> _R;

    /* Identity Matrix*/
    Matrix<N, N> _I;

    /**
     * Acceleration Transition Matrix
     * How the acceleration changes from one state to the other
     * considering a linearly increasing speed
     */
    Matrix<N, M> _G;

    /**
     * Acceleration Variance Matrix
     * How much we expect the acceleration to change
     */
    Matrix<N, N> _Q;

    /* Observation Matrix */
    Matrix<M, N> _H;

    /* Offset of state variables */
    const int X = 0, V = 1;

public:
    KalmanFilter(Vector<N> x, Matrix<M, M> measureVariance, T accVariance)
            : _F(), _S(), _H(), _R(measureVariance), _Q(), _G(), _x(x)
    {
        static_assert(floating_point,
                      "Please use one of the floating point types: float, "
                      "double, long double");

        /* Initialize observation Matrix. This is the same for entire computation*/
        _H.setZero();
        for (int i = 0; i < M; ++i) { _H(i, i * M) = 1; }

        // _H(0, 0) = 1;
        // _H(1, 2) = 1;

        _I.setIdentity();
        _S = _I;
        _Q = _I * accVariance;

        std::cout << "S: \n" << _S << std::endl;
        std::cout << "R: \n" << _R << std::endl;
        std::cout << "H: \n" << _H << std::endl;
        std::cout << "I: \n" << _I << std::endl;
    }

    void Predict(const T dt)
    {
        /* Initialize State Transition Matrix */
        _F.setIdentity();
        /* Initialize Acceleration Transition Matrix */
        _G.setZero();
        for (int i = 0; i < M; ++i) {
            _F(X + i * M, V + i * M) = dt;
            _G(X + i * M, i) = 0.5 * dt * dt;
            _G(V + i * M, i) = dt;
        }

        // std::cout << "F: \n" << _F << std::endl;
        // std::cout << "G: \n" << _G << std::endl;

        _x = _F * _x;
        _S = _F * _S * _F.transpose() + _G * _G.transpose() * _Q;
    }

    void Update(const Vector<M>& meas)
    {
        const Vector<M> y = meas - _H * _x;

        /* Gain matrix */
        const Matrix<N, M> _K = _S * _H.transpose() * (_H * _S * _H.transpose() + _R).inverse();

        /* Update states */
        _x += _K * y;

        /* Update State Covariance Matrix */
        _S = (_I - _K * _H) * _S;
    }

    void PredictAndUpdate(const Vector<M>& meas, const T dt)
    {
        this->Predict(dt);
        this->Update(meas);
    }

    Vector<N> GetApprox() const
    {
        return _x;
    }

    Matrix<N, N> GetCov() const
    {
        return _S;
    }

    T GetXPos() const
    {
        return _x[0];
    }

    T GetXSpeed() const
    {
        return _x[1];
    }

    T GetYPos() const
    {
        return _x[2];
    }

    T GetYSpeed() const
    {
        return _x[3];
    }
};

#endif /* KALMAN_FILTER_HEADER_HPP */