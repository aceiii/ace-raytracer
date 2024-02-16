#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "colour.h"
#include "vec3.h"

class texture {
public:
    virtual ~texture() = default;
    virtual colour value(double u, double v, const point3& p) const = 0;
};

class solid_colour : public texture {
public:
    solid_colour(colour c) : colour_value(c) {}
    solid_colour(double r, double g, double b) : solid_colour(colour(r, g, b)) {}

    colour value(double u, double v, const point3& p) const override {
        return colour_value;
    }

private:
    colour colour_value;
};

#endif//__TEXTURE_H__
