#ifndef __RAY_H__
#define __RAY_H__

#include "vec3.h"

class ray {
public:
    ray() {}
    ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction), tm(0){}

    ray(const point3& origin, const vec3& direction, double time = 0.0)
        : orig(origin), dir(direction), tm(time)
    {}

    inline point3 origin() const { return orig; }
    inline vec3 direction() const { return dir; }
    inline double time() const { return tm; }

    inline point3 at(double t) const {
        return orig + t * dir;
    }

private:
    point3 orig;
    vec3 dir;
    double tm;
};

#endif//__RAY_H__
