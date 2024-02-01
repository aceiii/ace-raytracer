#ifndef __RTWEEKEND_H__
#define __RTWEEKEND_H__

#include <cmath>
#include <limits>
#include <memory>

// Usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility functions
inline double degrees_to_radius(double degrees) {
    return degrees * pi / 180.0;
}

// Common headers
#include "ray.h"
#include "vec3.h"

#endif//__RTWEEKEND_H__
