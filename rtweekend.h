#ifndef __RTWEEKEND_H__
#define __RTWEEKEND_H__

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <random>

// Usings
using std::unique_ptr;
using std::shared_ptr;
using std::make_shared;
using std::make_unique;
using std::sqrt;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility functions
inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

static std::random_device _rd;
static thread_local std::mt19937 _generator(_rd());

inline double random_double() {
    // returns a random real in [0,1)
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(_generator);
}

inline double random_double(double min, double max) {
    // returns a random real in [min, max)
    return min + (max - min) * random_double();
}

inline int random_integer(int min, int max) {
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(_generator);
}

#endif//__RTWEEKEND_H__
