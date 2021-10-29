#ifndef RAY_HEADER_HPP
#define RAY_HEADER_HPP 1
#pragma once

#include "vec3.hpp"

class ray
{
public:
    __device__ ray()
    {
    }
    __device__ ray(const vec3& a, const vec3& b)
    {
        A = a;
        B = b;
    }

    __device__ vec3 origin() const
    {
        return A;
    }
    __device__ vec3 direction() const
    {
        return B;
    }
    __device__ vec3 point_at_parameter(float t) const
    {
        return A + t * B;
    }

private:
    /**
     * \note vector origin
     */
    vec3 A;

    /**
     * \note vector direction
     */
    vec3 B;
};

#endif //! RAY_HEADER_HPP
