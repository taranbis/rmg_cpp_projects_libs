#ifndef _VEC3_HEADER_HPP_
#define _VEC3_HEADER_HPP_ 1
#pragma once

#include <iostream>
#include <math.h>

/**
 * \class vec3
 * \brief implements the functionality of a 3D vector
 *
 * All methods except operator>> and operator<< will be used on both host
 * (computer) and device (GPU)
 */
class vec3
{
    float e[3];

public:
    __host__ __device__ vec3() {}
    __host__ __device__ vec3(float e0, float e1, float e2)
    {
        e[0] = e0;
        e[1] = e1;
        e[2] = e2;
    }

    __host__ __device__ inline float* data()
    {
        return e;
    }

    __host__ __device__ inline float x() const
    {
        return e[0];
    }

    __host__ __device__ inline void x(float t)
    {
        e[0] = t;
    }

    __host__ __device__ inline float y() const
    {
        return e[1];
    }

    __host__ __device__ inline void y(float t)
    {
        e[1] = t;
    }

    __host__ __device__ inline float z() const
    {
        return e[2];
    }

    __host__ __device__ inline void z(float t)
    {
        e[2] = t;
    }

    __host__ __device__ inline float r() const
    {
        return e[0];
    }
    __host__ __device__ inline float g() const
    {
        return e[1];
    }
    __host__ __device__ inline float b() const
    {
        return e[2];
    }

    __host__ __device__ inline const vec3& operator+() const
    {
        return *this;
    }

    __host__ __device__ inline const vec3 operator-() const
    {
        return vec3(-e[0], -e[1], -e[2]);
    }

    /**
     * \note this should have const as we return a copy
     */
    __host__ __device__ inline float operator[](int i) const
    {
        return e[i];
    }

    /**
     * \note this should NOT have const as we return a reference and this can be
     * modified by the caller of our function
     */
    __host__ __device__ inline float& operator[](int i)
    {
        return e[i];
    }

    __host__ __device__ inline vec3& operator+=(const vec3& v)
    {
        e[0] += v.x();
        e[1] += v.y();
        e[2] += v.z();
        return *this;
    }

    __host__ __device__ inline vec3& operator*=(const vec3& v)
    {
        e[0] *= v.x();
        e[1] *= v.y();
        e[2] *= v.z();
        return *this;
    }

    __host__ __device__ inline vec3& operator/=(const vec3& v)
    {
        e[0] /= v.x();
        e[1] /= v.y();
        e[2] /= v.z();
        return *this;
    }

    __host__ __device__ inline vec3& operator-=(const vec3& v)
    {
        e[0] -= v.x();
        e[1] -= v.y();
        e[2] -= v.z();
        return *this;
    }

    __host__ __device__ inline vec3& operator*=(const float t)
    {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    __host__ __device__ inline vec3& operator/=(const float k)
    {
        e[0] /= k;
        e[1] /= k;
        e[2] /= k;
        return *this;
    }

    __host__ __device__ inline float length() const
    {
        return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
    }
    __host__ __device__ inline float squared_length() const
    {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    __host__ __device__ inline void make_unit_vector()
    {
        *this /= this->length();
    }
};

inline std::istream& operator>>(std::istream& is, vec3& t)
{
    is >> t.data()[0] >> t.data()[1] >> t.data()[2];
    return is;
}

inline std::ostream& operator<<(std::ostream& os, const vec3& t)
{
    os << t.x() << " " << t.y() << " " << t.z();
    return os;
}

__host__ __device__ inline vec3 operator+(const vec3& v1, const vec3& v2)
{
    return vec3(v1.x() + v2.x(), v1.y() + v2.y(), v1.z() + v2.z());
}

__host__ __device__ inline vec3 operator-(const vec3& v1, const vec3& v2)
{
    return vec3(v1.x() - v2.x(), v1.y() - v2.y(), v1.z() - v2.z());
}

__host__ __device__ inline vec3 operator*(const vec3& v1, const vec3& v2)
{
    return vec3(v1.x() * v2.x(), v1.y() * v2.y(), v1.z() * v2.z());
}

__host__ __device__ inline vec3 operator/(const vec3& v1, const vec3& v2)
{
    return vec3(v1.x() / v2.x(), v1.y() / v2.y(), v1.z() / v2.z());
}

__host__ __device__ inline vec3 operator*(float t, const vec3& v)
{
    return vec3(v.x() * t, v.y() * t, v.z() * t);
}

__host__ __device__ inline vec3 operator/(const vec3& v, float k)
{
    return vec3(v.x() / k, v.y() / k, v.z() / k);
}

__host__ __device__ inline vec3 operator*(const vec3& v, float t)
{
    return vec3(v.x() * t, v.y() * t, v.z() * t);
}

__host__ __device__ inline float dot(const vec3& v1, const vec3& v2)
{
    return v1.x() * v2.x() + v1.y() * v2.y() + v1.z() * v2.z();
}

__host__ __device__ inline vec3 cross(const vec3& v1, const vec3& v2)
{
    return vec3((v1.y() * v2.z() - v1.z() * v2.y()), (-(v1.x() * v2.z() - v1.z() * v2.z())),
                (v1.x() * v2.y() - v1.y() * v2.x()));
}

__host__ __device__ inline vec3 unit_vector(vec3 v)
{
    return v / v.length();
}

#endif //!_VEC3_HEADER_HPP_
