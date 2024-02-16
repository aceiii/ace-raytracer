#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "rtweekend.h"
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

class checker_texture : public texture {
public:
    checker_texture(double _scale, shared_ptr<texture> _even, shared_ptr<texture> _odd)
        : inv_scale(1.0 / _scale), even(_even), odd(_odd) {}

    checker_texture(double _scale, colour c1, colour c2)
        : inv_scale(1.0 / _scale), even(make_shared<solid_colour>(c1)), odd(make_shared<solid_colour>(c2)) {}

    colour value(double u, double v, const point3& p) const override {
        auto x_int = static_cast<int>(std::floor(inv_scale * p.x()));
        auto y_int = static_cast<int>(std::floor(inv_scale * p.y()));
        auto z_int = static_cast<int>(std::floor(inv_scale * p.z()));

        bool is_even = (x_int + y_int + z_int) % 2 == 0;
        return is_even ? even->value(u, v, p) : odd->value(u, v, p);
    }

private:
    double inv_scale;
    shared_ptr<texture> even;
    shared_ptr<texture> odd;
};

#endif//__TEXTURE_H__
