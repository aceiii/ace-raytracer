#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "rtweekend.h"
#include "colour.h"
#include "hittable.h"

class camera {
public:
    double  aspect_ratio = 1.0;
    int     image_width = 100;

    void render(const hittable& world) {
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j += 1) {
            std::clog << "\rScanlins remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; i += 1) {
                auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
                auto ray_direction = pixel_center - center;
                ray r(center, ray_direction);
                colour pixel_colour = ray_colour(r, world);
                write_colour(std::cout, pixel_colour);
            }
        }

        std::clog << "\rDone.                        \n";
    }

private:
    int     image_height;
    point3  center;
    point3  pixel00_loc;
    vec3    pixel_delta_u;
    vec3    pixel_delta_v;

    void initialize() {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        // Camera
        auto focal_length = 1.0;
        auto viewport_height = 2.0;
        auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);
        center = point3(0, 0, 0);

        // Vectors across and down the viewport edges
        auto viewport_u = vec3(viewport_width, 0, 0);
        auto viewport_v = vec3(0, -viewport_height, 0); // positive y is up

        // Vectors from pixel to pixel across and down
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate upper left pixel location
        auto viewport_upper_left = center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    colour ray_colour(const ray& r, const hittable& world) const {
        hit_record rec;
        if (world.hit(r, interval(0, infinity), rec)) {
            return 0.5 * (rec.normal + colour(1, 1, 1));
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return lerp(a, colour(1.0, 1.0, 1.0), colour(0.5, 0.7, 1.0));
    }

    colour lerp(double a, const colour& start, const colour& end) const {
        return (1.0 - a) * start + a * end;
    }
};

#endif//__CAMERA_H__
