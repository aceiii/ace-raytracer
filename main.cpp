#include "rtweekend.h"
#include "camera.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "bvh.h"
#include "texture.h"
#include "raylib_window.h"

#include <iostream>
#include <thread>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/core.h>
#include <stb_image_write.h>
#include <taskflow/taskflow.hpp>
#include <argparse/argparse.hpp>

struct scene_info {
    hittable_list world;
    double vfov;
    point3 lookfrom;
    point3 lookat;
    vec3 vup;
    double defocus_angle;
    double focus_dist;
};

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

scene_info random_spheres() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(0.32, colour(0.2, 0.3, 0.1), colour(0.9, 0.9, 0.9));
    auto ground_material = make_shared<lambertian>(checker);
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
                    auto center2 = center + vec3(0, random_double(0, 0.5), 0);
                    world.add(make_shared<sphere>(center, center2, 0.2, sphere_material));
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

    scene_info scene;
    scene.world = world;
    scene.vfov = 20;
    scene.lookfrom = point3(13, 2, 3);
    scene.lookat = point3(0, 0, 0);
    scene.vup = vec3(0, 1, 0);
    scene.defocus_angle = 0.6;
    scene.focus_dist = 10.0;

    return scene;
}

scene_info two_spheres() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(0.8, colour(0.2, 0.3, 0.1), colour(0.9, 0.9, 0.9));
    auto checker_material = make_shared<lambertian>(checker);

    world.add(make_shared<sphere>(point3(0, -10, 0), 10, checker_material));
    world.add(make_shared<sphere>(point3(0, 10, 0), 10, checker_material));

    scene_info scene;
    scene.world = world;
    scene.vfov = 20;
    scene.lookfrom = point3(13, 2, 3);
    scene.lookat = point3(0, 0, 0);
    scene.vup = vec3(0, 1, 0);
    scene.defocus_angle = 0;
    scene.focus_dist = 10.0;

    return scene;
}

scene_info earth() {
    hittable_list world = world;

    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

    world.add(globe);

    scene_info scene;
    scene.world = world;
    scene.vfov = 20;
    scene.lookfrom = point3(0, 0, 12);
    scene.lookat = point3(0, 0, 0);
    scene.vup = vec3(0, 1, 0);
    scene.defocus_angle = 0;
    scene.focus_dist = 10.0;

    return scene;
}

scene_info get_scene(int n) {
    switch (n) {
        case 1: return random_spheres();
        case 2: return two_spheres();
        case 3: return earth();
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

    spdlog::debug("Args:");
    spdlog::debug("  Num threads: {}", program.get<int>("--threads"));
    spdlog::debug("  Log level: {}", program.get("--log-level"));

    spdlog::info("Starting raytracer!");

    auto scene = get_scene(1);
    auto world = hittable_list(make_shared<bvh_node>(scene.world));

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 1024;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.vfov = scene.vfov;
    cam.lookfrom = scene.lookfrom;
    cam.lookat = scene.lookat;
    cam.vup = scene.vup;
    cam.defocus_angle = scene.defocus_angle;
    cam.focus_dist = scene.focus_dist;
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
