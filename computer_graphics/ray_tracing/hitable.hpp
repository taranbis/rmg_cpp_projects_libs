#ifndef _HITABLE_HEADER_HPP_
#define _HITABLE_HEADER_HPP_ 1
#pragma once

#include "ray.hpp"

struct hit_record {
    float t;
    vec3  p;
    vec3  normal;
};

class hitable
{
public:
    __device__ virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
};

#endif //! _HITABLE_HEADER_HPP_