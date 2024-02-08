#ifndef __RTWEEKEND_H__
#define __RTWEEKEND_H__

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <random>

// Usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility functions
inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double random_double() {
    // returns a random real in [0,1)
    static thread_local std::random_device rd;
    static thread_local std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static thread_local std::mt19937 generator(rd());
    return distribution(generator);
}

inline double random_double(double min, double max) {
    // returns a random real in [min, max)
    return min + (max - min) * random_double();
}

// Common headers
#include "interval.h"
#include "ray.h"
#include "vec3.h"
#include "colour.h"

#endif//__RTWEEKEND_H__
