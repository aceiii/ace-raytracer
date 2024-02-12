#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "rtweekend.h"
#include "colour.h"
#include "hittable.h"
#include "material.h"
#include "bitmap.h"
#include "timing.h"

#include <thread>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/color.h>
#include <taskflow/taskflow.hpp>

struct dimensions {
    int width;
    int height;
};

class camera {
public:
    double  aspect_ratio = 1.0;
    int     image_width = 100;
    int     samples_per_pixel = 10;
    int     max_depth = 10;

    double vfov = 90;
    point3 lookfrom = point3(0, 0, -1); // center of camera
    point3 lookat = point3(0, 0, 0); // target
    vec3 vup = vec3(0, 1, 0); // camera-relative "up" direction

    double defocus_angle = 0; // variation of angle of rays through each pixel
    double focus_dist = 10; // distance from camera lookfrom to perfect focus

    void init() {
        initialize();
    }

    dimensions get_image_dimensions() const {
        return { image_width, image_height };
    }

    void render(const hittable& world, tf::Subflow& subflow, std::shared_ptr<bitmap> out_bmp) {
        using namespace fmt;

        timer time;

        spdlog::info("Rendering scene...");

        stopped = false;
        done = false;

        for (int y = 0; y < image_height && !stopped; y += 1) {
            timer row_time;

            for (int x = 0; x < image_width && !stopped; x += 1) {
                auto task = subflow.emplace([&, x, y, out_bmp, this]() {
                    if (stopped) {
                        return;
                    }

                    colour pixel_colour(0, 0, 0);
                    for (int sample = 0; sample < samples_per_pixel; sample += 1) {
                        ray r = get_ray(x, y);
                        pixel_colour += ray_colour(r, max_depth, world);
                    }

                    pixel& px = out_bmp->pixel_at(x, y);
                    write_colour(px, pixel_colour, samples_per_pixel);
                    spdlog::trace("output pixel at ({}, {}) -> ({}, {}, {})", x, y, px.r, px.g, px.b);
                });

                auto rand_priority = static_cast<tf::TaskPriority>(random_integer(0, static_cast<int>(tf::TaskPriority::MAX) - 1));
                task.priority(rand_priority);
            }
        }

        subflow.join();

        if (stopped) {
            return;
        }

        done = true;
        auto diff = time.duration<timer::milliseconds>();
        spdlog::info("Rendering completed in {}", format(fg(color::aqua), "{:.2f}ms", diff));
    }

    void cancel() {
        stopped = true;
        done = false;
    }

    bool rendering() const {
        return !stopped;
    }

    bool complete() const {
        return done;
    }

private:
    int     image_height;
    point3  center;
    point3  pixel00_loc;
    vec3    pixel_delta_u;
    vec3    pixel_delta_v;
    vec3    u, v, w; // camera frame basis vectors
    vec3    defocus_disk_u;
    vec3    defocus_disk_v;

    bool stopped = true;
    bool done = false;

    void initialize() {
        using namespace fmt;

        spdlog::info("Initializing camera:");

        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = lookfrom;

        // Camera
        // auto focal_length = (lookfrom - lookat).length();
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

        // calculate basis vectors for camera
        w = unit_vector(lookfrom - lookat); // opposite direction of lookat
        u = unit_vector(cross(vup, w)); // camera right vector
        v = cross(w, u); // camera up vector

        // Vectors across and down the viewport edges
        vec3 viewport_u = viewport_width * u;
        vec3 viewport_v = viewport_height * -v; // positive y is up

        // Vectors from pixel to pixel across and down
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate upper left pixel location
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate focus disk basis vectors
        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;

        spdlog::info("  image dimensions: {}", format(fg(color::aqua), "{} x {}", image_width, image_height));
        spdlog::info("  samples per pixel: {}", styled(samples_per_pixel, fg(color::aqua)));
    }

    colour ray_colour(const ray& r, int depth, const hittable& world) const {
        hit_record rec;

        // no more bounces, no more light
        if (depth <= 0) {
            return colour(0, 0, 0);
        }

        if (world.hit(r, interval(0.001, infinity), rec)) {
            ray scattered;
            colour attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered)) {
                return attenuation * ray_colour(scattered, depth-1, world);
            }
            return colour(0, 0, 0);
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return lerp(a, colour(1.0, 1.0, 1.0), colour(0.5, 0.7, 1.0));
    }

    ray get_ray(int i, int j) const {
        // get randomly sampled camera ray for pixel at location i,j
        // originate from camera defocus disk

        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        auto pixel_sample = pixel_center + pixel_sample_square();

        auto ray_origin = defocus_angle <= 0 ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    point3 defocus_disk_sample() const {
        // random point within defocus disk
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    vec3 pixel_sample_square() const {
        // return random point in square surrounding pixel at origin
        auto px = -0.5 * random_double();
        auto py = -0.5 * random_double();
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }
};

#endif//__CAMERA_H__
