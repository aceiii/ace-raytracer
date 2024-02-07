#ifndef __COLOUR_H__
#define __COLOUR_H__

#include "vec3.h"
#include "pixel.h"

using colour = vec3;

inline double linear_to_gamma(double linear_component) {
    return sqrt(linear_component);
}

void write_colour(pixel &out, colour pixel_colour, int samples_per_pixel) {
    auto r = pixel_colour.x();
    auto g = pixel_colour.y();
    auto b = pixel_colour.z();

    auto scale = 1.0 / samples_per_pixel;
    r *= scale;
    g *= scale;
    b *= scale;

    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    static const interval intensity(0.0, 0.99999);

    out.r = static_cast<int>(256 * intensity.clamp(r));
    out.g = static_cast<int>(256 * intensity.clamp(g));
    out.b = static_cast<int>(256 * intensity.clamp(b));
    out.a = 255;
}

#endif//__COLOUR_H__
