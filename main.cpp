#include "rtweekend.h"
#include "camera.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "raylib_window.h"

#include <iostream>
#include <thread>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/core.h>
#include <stb_image_write.h>
#include <taskflow/taskflow.hpp>
#include <argparse/argparse.hpp>

static void set_logging_level(const std::string& level) {
    if (level == "trace") {
        spdlog::set_level(spdlog::level::trace);
    } else if (level == "debug") {
        spdlog::set_level(spdlog::level::debug);
    } else if (level == "info") {
        spdlog::set_level(spdlog::level::info);
    } else if (level == "warn") {
        spdlog::set_level(spdlog::level::warn);
    } else if (level == "err") {
        spdlog::set_level(spdlog::level::err);
    } else if (level == "off") {
        spdlog::set_level(spdlog::level::off);
    } else {
        fmt::print("Invalid log: {}", level);
    }
}

int main(int argc, char* argv[]) {
    spdlog::set_level(spdlog::level::info);

    argparse::ArgumentParser program("ace-raytracer", "0.0.1");

    int default_thread_count = std::thread::hardware_concurrency();
    program.add_argument("-t", "--threads")
        .help("display the square of a given number")
        .default_value(default_thread_count)
        .nargs(1)
        .scan<'i', int>();

    program.add_argument("--log-level")
        .help("Set the verbosity for logging")
        .default_value(std::string("info"))
        .choices("trace", "debug", "info", "warn", "err", "critical", "off")
        .nargs(1);

    try {
        program.parse_args(argc, argv);
    } catch(const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    set_logging_level(program.get("--log-level"));

    spdlog::debug("Args:\n");
    spdlog::debug("  Num threads: {}\n", program.get<int>("--threads"));
    spdlog::debug("  Log level: {}\n", program.get("--log-level"));

    spdlog::info("Starting raytracer!");

    hittable_list world;

    auto ground_material = make_shared<lambertian>(colour(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            auto center = point3(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = colour::random() * colour::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = colour::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dieletric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dieletric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(colour(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(colour(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 800;
    cam.samples_per_pixel = 10;
    cam.max_depth = 50;
    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);
    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;
    cam.init();

    auto dims = cam.get_image_dimensions();
    auto bmp = std::make_shared<bitmap>(dims.width, dims.height);

    raylib_window rw;
    rw.num_threads = program.get<int>("--threads");
    rw.log_level = program.get("--log-level");
    rw.run(cam, world, bmp);

    spdlog::info("Done!");

    return 0;
}
