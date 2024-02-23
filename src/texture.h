#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "rtweekend.h"
#include "colour.h"
#include "vec3.h"
#include "perlin.h"
#include "interval.h"
#include "rtw_stb_image.h"

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

class image_texture : public texture {
public:
    image_texture(const char* filename) : image(filename) {}

    colour value(double u, double v, const point3& p) const override {
        // if no texture data return solid cyan for debugging purposes
        if (image.height() <= 0) {
            return colour(0, 1, 1);
        }

        // clamp input to [0,1] x [1,0]
        u = interval(0, 1).clamp(u);
        v = 1.0 - interval(0, 1).clamp(v); // flip v to image coordinates

        auto i = static_cast<int>(u * image.width());
        auto j = static_cast<int>(v * image.height());
        auto pixel = image.pixel_data(i, j);

        auto colour_scale = 1.0 / 255.0;
        return colour(colour_scale * pixel[0], colour_scale * pixel[1], colour_scale * pixel[2]);
    }

private:
    rtw_image image;
};

class noise_texture : public texture {
public:
    noise_texture() {}

    noise_texture(double sc) : scale(sc) {}

    colour value(double u, double v, const point3& p) const override {
        // return colour(1, 1, 1) * 0.5 * (1.0 + noise.noise(scale * p));

        auto s = scale * p;
        // return colour(1, 1, 1) * noise.turb(s);
        return colour(1, 1, 1) * 0.5 * (1 - sin(s.z() + 10 * noise.turb(s)));
    }

private:
    perlin noise;
    double scale = 1;
};


#endif//__TEXTURE_H__
