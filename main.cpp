#include "rtweekend.h"
#include "camera.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "bvh.h"
#include "texture.h"
#include "quad.h"
#include "constant_medium.h"
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
    double vfov = 20;
    point3 lookfrom = point3(1, 0, 1);
    point3 lookat = point3(0, 0, 0);
    vec3 vup = vec3(0, 1, 0);
    double defocus_angle = 0;
    double focus_dist = 10;
    colour background = { 0.5, 0.5, 0.5 };
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
    scene.background = colour(0.7, 0.8, 1.0);

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
    scene.background = colour(0.7, 0.8, 1.0);

    return scene;
}

scene_info earth() {
    hittable_list world;

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
    scene.background = colour(0.7, 0.8, 1.0);

    return scene;
}

scene_info two_perlin_spheres() {
    scene_info scene;

    auto pertext = make_shared<noise_texture>(4);
    scene.world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    scene.world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    scene.vfov = 20;
    scene.lookfrom = point3(13, 2, 3);
    scene.lookat = point3(0, 0, 0);
    scene.vup = vec3(0, 1, 0);
    scene.defocus_angle = 0;
    scene.focus_dist = 10.0;
    scene.background = colour(0.7, 0.8, 1.0);

    return scene;
}

scene_info quads() {
    scene_info scene;

    auto left_red = make_shared<lambertian>(colour(1.0, 0.2, 0.2));
    auto back_green = make_shared<lambertian>(colour(0.2, 1.0, 0.2));
    auto right_blue = make_shared<lambertian>(colour(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<lambertian>(colour(1.0, 0.5, 0.0));
    auto lower_teal = make_shared<lambertian>(colour(0.2, 0.8, 0.8));

    scene.world.add(make_shared<quad>(point3(-3, -2, 5), vec3(0, 0, -4), vec3(0, 4, 0), left_red));
    scene.world.add(make_shared<quad>(point3(-2, -2, 0), vec3(4, 0,  0), vec3(0, 4, 0), back_green));
    scene.world.add(make_shared<quad>(point3( 3, -2, 1), vec3(0, 0,  4), vec3(0, 4, 0), right_blue));
    scene.world.add(make_shared<quad>(point3(-2,  3, 1), vec3(4, 0,  0), vec3(0, 0, 4), upper_orange));
    scene.world.add(make_shared<quad>(point3(-2, -3, 5), vec3(4, 0,  0), vec3(0, 0,-4), lower_teal));

    scene.vfov = 80;
    scene.lookfrom = point3(0, 0, 9);
    scene.lookat = point3(0, 0, 0);
    scene.vup = vec3(0, 1, 0);
    scene.defocus_angle = 0;
    scene.focus_dist = 10.0;
    scene.background = colour(0.7, 0.8, 1.0);

    return scene;
}

scene_info simple_light() {
    scene_info scene;

    auto pertext = make_shared<noise_texture>(4);
    scene.world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    scene.world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(colour(4, 4, 4));
    scene.world.add(make_shared<sphere>(point3(0, 7, 0), 2, difflight));
    scene.world.add(make_shared<quad>(point3(3, 1, -2), vec3(2, 0, 0), vec3(0, 2, 0), difflight));

    scene.vfov = 20;
    scene.lookfrom = point3(26, 3, 6);
    scene.lookat = point3(0, 2, 0);
    scene.vup = vec3(0, 1, 0);
    scene.defocus_angle = 0;
    scene.background = colour(0, 0, 0);

    return scene;
}

scene_info cornell_box() {
    scene_info scene;

    auto red = make_shared<lambertian>(colour(0.65, 0.05, 0.05));
    auto white = make_shared<lambertian>(colour(0.73, 0.73, 0.73));
    auto green = make_shared<lambertian>(colour(0.12, 0.45, 0.15));
    auto light = make_shared<diffuse_light>(colour(15, 15, 15));

    scene.world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
    scene.world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
    scene.world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light));
    scene.world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    scene.world.add(make_shared<quad>(point3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), white));
    scene.world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

    shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    scene.world.add(box1);

    shared_ptr<hittable> box2 = box(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));
    scene.world.add(box2);

    scene.vfov = 40;
    scene.lookfrom = point3(278, 278, -800);
    scene.lookat = point3(278, 278, 0);
    scene.vup = point3(0, 1, 0);
    scene.defocus_angle = 0;
    scene.background = colour(0, 0, 0);

    return scene;
}

scene_info cornell_smoke() {
    scene_info scene;

    auto red = make_shared<lambertian>(colour(0.65, 0.05, 0.05));
    auto white = make_shared<lambertian>(colour(0.73, 0.73, 0.73));
    auto green = make_shared<lambertian>(colour(0.12, 0.45, 0.15));
    auto light = make_shared<diffuse_light>(colour(15, 15, 15));

    scene.world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
    scene.world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
    scene.world.add(make_shared<quad>(point3(113, 554, 127), vec3(330, 0, 0), vec3(0, 0, 305), light));
    scene.world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    scene.world.add(make_shared<quad>(point3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), white));
    scene.world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

    shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));

    shared_ptr<hittable> box2 = box(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));

    scene.world.add(make_shared<constant_medium>(box1, 0.01, colour(0, 0, 0)));
    scene.world.add(make_shared<constant_medium>(box2, 0.01, colour(1, 1, 1)));

    scene.vfov = 40;
    scene.lookfrom = point3(278, 278, -800);
    scene.lookat = point3(278, 278, 0);
    scene.vup = point3(0, 1, 0);
    scene.defocus_angle = 0;
    scene.background = colour(0, 0, 0);
    return scene;
}

scene_info final_scene() {
    scene_info scene;

    hittable_list boxes1;
    auto ground = make_shared<lambertian>(colour(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i += 1) {
        for (int j = 0; j < boxes_per_side; j += 1) {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1, 101);
            auto z1 = z0 + w;

            boxes1.add(box(point3(x0, y0, z0), point3(x1, y1, z1), ground));
        }
    }

    scene.world.add(make_shared<bvh_node>(boxes1));

    auto light = make_shared<diffuse_light>(colour(7, 7, 7));
    scene.world.add(make_shared<quad>(point3(123, 554, 147), vec3(300, 0, 0), vec3(0, 0, 265), light));

    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30, 0, 0);
    auto sphere_material = make_shared<lambertian>(colour(0.7, 0.3, 0.1));
    scene.world.add(make_shared<sphere>(center1, center2, 50, sphere_material));

    scene.world.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dieletric>(1.5)));
    scene.world.add(make_shared<sphere>(point3(0, 150, 145), 50, make_shared<metal>(colour(0.8, 0.8, 0.9), 1.0)));

    auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dieletric>(1.5));
    scene.world.add(boundary);
    scene.world.add(make_shared<constant_medium>(boundary, 0.2, colour(0.2, 0.4, 0.9)));

    boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dieletric>(1.5));
    scene.world.add(make_shared<constant_medium>(boundary, 0.0001, colour(1, 1, 1)));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
    scene.world.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));

    auto pertext = make_shared<noise_texture>(0.1);
    scene.world.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    auto white = make_shared<lambertian>(colour(0.73, 0.73, 0.73));
    int ns = 1000;
    for (int j = 0; j < ns; j += 1) {
        boxes2.add(make_shared<sphere>(point3::random(0, 165), 10, white));
    }

    scene.world.add(make_shared<translate>(make_shared<rotate_y>(make_shared<bvh_node>(boxes2), 15), vec3(-100, 270, 395)));

    scene.vfov = 40;
    scene.lookfrom = point3(478, 278, -600);
    scene.lookat = point3(278, 278, 0);
    scene.vup = point3(0, 1, 0);
    scene.defocus_angle = 0;
    scene.background = colour(0, 0, 0);
    return scene;
}

scene_info get_scene(int n) {
    switch (n) {
        case 1: return random_spheres();
        case 2: return two_spheres();
        case 3: return earth();
        case 4: return two_perlin_spheres();
        case 5: return quads();
        case 6: return simple_light();
        case 7: return cornell_box();
        case 8: return cornell_smoke();
        case 9: return final_scene();
    }
    return {};
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
    cam.background = scene.background;
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
