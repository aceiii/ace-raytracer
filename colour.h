#ifndef __COLOUR_H__
#define __COLOUR_H__

#include "vec3.h"

#include <iostream>

using colour = vec3;

void write_colour(std::ostream &out, colour pixel_colour) {
    out << static_cast<int>(255.999 * pixel_colour.x()) << ' '
        << static_cast<int>(255.999 * pixel_colour.y()) << ' '
        << static_cast<int>(255.999 * pixel_colour.z()) << '\n';
}

#endif//__COLOUR_H__
