#ifndef _SPHERE_HEADER_HPP_
#define _SPHERE_HEADER_HPP_ 1
#pragma once

#include "hitable.hpp"

class sphere : public hitable
{
private:
    vec3  centre;
    float radius;

public:
    __device__ sphere() {}
    __device__ sphere(vec3 cen, float r) : centre(cen), radius(r) {}

    __device__ bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override final;
    __device__ ~sphere() {}
};

__device__ bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
    vec3  oc = r.origin() - centre;
    float a = dot(r.direction(), r.direction());
    float b = 2.0f * dot(oc, r.direction());
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - a * c;
    if (discriminant > 0.0f) {
        float temp = (-b - sqrt(discriminant)) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - centre) / radius;
            return true;
        }
        temp = (-b + sqrt(discriminant)) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - centre) / radius;
            return true;
        }
    }
    return false;
}

#endif //! _SPHERE_HEADER_HPP_
