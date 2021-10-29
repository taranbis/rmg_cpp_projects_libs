#ifndef FORKLIFT_HEADER_HPP
#define FORKLIFT_HEADER_HPP 1
#pragma once

#include <atomic>
#include <chrono>
#include <iostream>
#include <random>
#include <thread>

#define DEB(x)     std::cout << #x << " =  " << x << ";\t "
#define DEBBOOL(x) std::cout << std::boolalpha << #x << " =  " << x << ";\t "
#define NEWLINE()  std::cout << std::endl
#define DEBLINE()  std::cout << __LINE__ << std::endl;
#define DEBFILE()  std::cout << __FILE__ << std::endl;
#define PAUSE_FUNCTION(x)                                      \
    static int counter = 0;                                    \
    counter++;                                                 \
    if (counter == x)                                          \
    {                                                          \
        std::this_thread::sleep_for(std::chrono::seconds(10)); \
        counter = 0;                                           \
    }

template <typename T>
inline T abs(T x)
{
    return ((x < 0) ? (-x) : x);
}

/**
 * @class Movement
 * @brief Implements a movement type
 *  We use a class here as maybe we want to extend the types of movement and select at run-time a
 *  different type of movement. We will be able to use this as a base class.
 */
template <typename T>
class Movement
{
public:
    /* If a = 0, it is linear movement. No need to select depending on type */
    std::tuple<T, T, T> operator()(T x, T v, T a, T dt)
    {
        v += dt * a;
        x += dt * v + a * dt * dt / 2;
        return {x, v, a};
    }
};

/**
 * @class Forklift
 * @brief Models a real life autonomous forklift
 * @param T numeric floating point type
 */
template <typename T, bool floating_point = std::is_floating_point<T>::value>
class Forklift
{
public:
    /* acceleration, decceleration (braking) and max speed values for a forklift, known from specifications */
    static constexpr T acc = 1.0, decc = -2.5;
    static constexpr T vMax = 10;

private:
    uint32_t _id = 0;

    /**
     *  Current Position
     *  std::atomic is used to access it from a reading thread
     *  We can use std::memory_relaxed needs as we don't care
     *  about the order of operation.
     */
    std::atomic<T> _currX{0}, _currY{0}, _currZ{0};

    // TODO: should make this atomic as well
    std::chrono::time_point<std::chrono::high_resolution_clock> _currTime{
        std::chrono::high_resolution_clock::now()};

    std::atomic<T> _v{0.0};

    template <typename D>
    struct Trajectory
    {
        D dist;
        D sinAngle;
        D cosAngle;
    };

    Trajectory<T> _trajectory;
    Movement<T>   _movement;

    bool _brake{false};
    bool _moving{false};

public:
    Forklift(uint32_t id) : _id(id)
    {
        static_assert(floating_point,
                      "Please use one of the floating point types: float, "
                      "double, long double");
    }

    Forklift(uint32_t id, T x, T y, T z)
        : _id(id), _currX(x), _currY(y), _currZ(z)
    {
        static_assert(floating_point,
                      "Please use one of the floating point types: float, "
                      "double, long double");
    }

    ~Forklift() = default;

    /* not copyable */
    Forklift(const Forklift& other) = delete;
    Forklift& operator=(const Forklift& other) = delete;

    /* but movable */
    Forklift(Forklift&& other) = default;
    Forklift& operator=(Forklift&& other) = default;

    void SetDist(T d)
    {
        _trajectory.dist = d;
    }

    void SetSpeed(T v)
    {
        _v.store(std::memory_order_relaxed);
    }

    uint32_t GetTag() const
    {
        return _id;
    }

    T GetXPosition() const
    {
        return _currX.load(std::memory_order_relaxed);
    }

    T GetYPosition() const
    {
        return _currY.load(std::memory_order_relaxed);
    }

    T GetSpeed() const
    {
        return _v.load(std::memory_order_relaxed);
    }

    T GetXSpeed() const
    {
        return abs(_v.load(std::memory_order_relaxed) * _trajectory.cosAngle);
    }

    T GetYSpeed() const
    {
        return abs(_v.load(std::memory_order_relaxed) * _trajectory.sinAngle);
    }

    T GetTime() const
    {
        return ((T)std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::high_resolution_clock::now() - _currTime)
                    .count()) /
               1000;
    }

    void Wait(int numSecs)
    {
        _moving = false;
        _v.store(0, std::memory_order_relaxed);
        std::this_thread::sleep_for(std::chrono::seconds(numSecs));
    }

    void MoveToPosition(T destX, T destY)
    {
        _moving = true;
        _brake  = false;

        /* compute trajectory to target: distance and angle from current position */
        _trajectory.dist =
            sqrt((destX - _currX.load(std::memory_order_relaxed)) *
                     (destX - _currX.load(std::memory_order_relaxed)) +
                 (destY - _currY.load(std::memory_order_relaxed)) *
                     (destY - _currY.load(std::memory_order_relaxed)));
        _trajectory.cosAngle =
            (destX - _currX.load(std::memory_order_relaxed)) / _trajectory.dist;
        _trajectory.sinAngle =
            (destY - _currY.load(std::memory_order_relaxed)) / _trajectory.dist;

        /* this condition is sufficient to stop on target */
        while (_trajectory.dist > 0.03)
        {
            // maybe use a last update variable
            _currTime = std::chrono::high_resolution_clock::now();
            ExecuteMove(destX, destY);
        }
    }

private:
    void ExecuteMove(T destX, T destY)
    {
        /* dt will always be 0 if we don't do some waiting */
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        /* This should be in seconds */
        T dt = ((T)std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::high_resolution_clock::now() - _currTime)
                    .count()) /
               1000;

        DEB(_currX.load(std::memory_order_relaxed));
        DEB(destX);
        DEB(_currY.load(std::memory_order_relaxed));
        DEB(destY);
        DEB(GetXSpeed());
        DEB(GetYSpeed());
        NEWLINE();

        T v = _v.load(std::memory_order_relaxed);

        if (v * v >= 2 * abs(decc) * _trajectory.dist)
        {
            Brake(dt);
            /* Once it starts breaking it will stop only on target */
            _brake = true;
            return;
        }

        if (!_brake)
        {
            /* if V != Vmax m/s, speed increases linearly */
            if (v <= vMax)
            {
                Accelerate(dt);
                return;
            }
        }

        /* If I end up here it is linear movement with const speed, as the acceleration provided is 0 */
        auto [dtmp, vtmp, a] = _movement(0, v, 0, dt);
        UpdatePosition(dtmp);
    }

    void Brake(T dt)
    {
        auto [dtmp, v, a] = _movement(0, _v.load(std::memory_order_relaxed), decc, dt);
        _v.store(v, std::memory_order_relaxed);
        UpdatePosition(dtmp);
    }

    void Accelerate(T dt)
    {
        auto [dtmp, v, a] = _movement(0, _v.load(std::memory_order_relaxed), acc, dt);
        _v.store(v, std::memory_order_relaxed);
        UpdatePosition(dtmp);
    }

    void UpdatePosition(T dtmp)
    {
        _currX.store(
            (_currX.load(std::memory_order_relaxed) + dtmp * _trajectory.cosAngle),
            std::memory_order_relaxed);
        _currY.store(
            (_currY.load(std::memory_order_relaxed) + dtmp * _trajectory.sinAngle),
            std::memory_order_relaxed);
        _trajectory.dist -= dtmp;
    }
};

#endif /* FORKLIFT_HEADER_HPP */
