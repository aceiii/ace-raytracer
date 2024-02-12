#include "raylib_window.h"

#include <raylib.h>

static void set_logging_level(const std::string& level) {
    if (level == "trace") {
        SetTraceLogLevel(LOG_TRACE);
    } else if (level == "debug") {
        SetTraceLogLevel(LOG_DEBUG);
    } else if (level == "info") {
        SetTraceLogLevel(LOG_INFO);
    } else if (level == "warn") {
        SetTraceLogLevel(LOG_WARNING);
    } else if (level == "err") {
        SetTraceLogLevel(LOG_ERROR);
    } else if (level == "off") {
        SetTraceLogLevel(LOG_NONE);
    } else {
        fmt::print("Invalid log: {}", level);
    }
}

void raylib_window::run(camera& cam, const hittable_list& world, std::shared_ptr<bitmap> bmp) {
    set_logging_level(log_level);

    tf::Executor executor(num_threads);
    tf::Taskflow taskflow;

    auto start_render = taskflow.emplace([&world, &cam, bmp](tf::Subflow subflow) {
        cam.render(world, subflow, bmp);
    });

    dimensions dims = cam.get_image_dimensions();
    InitWindow(dims.width, dims.height, "Ace Raytracer!");

    Camera3D cam3d;
    cam3d.fovy = cam.vfov;
    cam3d.position = Vector3 {
        static_cast<float>(cam.lookfrom.x()),
        static_cast<float>(cam.lookfrom.y()),
        static_cast<float>(cam.lookfrom.z())
    };
    cam3d.target = Vector3 {
        static_cast<float>(cam.lookat.x()),
        static_cast<float>(cam.lookat.y()),
        static_cast<float>(cam.lookat.z())
    };
    cam3d.up = Vector3 {
        static_cast<float>(cam.vup.x()),
        static_cast<float>(cam.vup.y()),
        static_cast<float>(cam.vup.z())
    };

    cam3d.projection = CAMERA_PERSPECTIVE;

    tf::Future<void> future;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) {
            break;
        }

        if (IsKeyPressed(KEY_SPACE)) {
            if (cam.rendering() && !cam.complete()) {
                cam.cancel();
            } else if (cam.complete()) {
                bmp->clear();
                cam.cancel();
                future.wait();
            } else {
                bmp->clear();
                cam.init();
                future = executor.run(taskflow);
            }
        }

        if (!cam.rendering()) {
            UpdateCamera(&cam3d, CAMERA_FIRST_PERSON);
            cam.lookat = vec3 {
                static_cast<double>(cam3d.target.x),
                static_cast<double>(cam3d.target.y),
                static_cast<double>(cam3d.target.z)
            };
            cam.lookfrom = vec3 {
                static_cast<double>(cam3d.position.x),
                static_cast<double>(cam3d.position.y),
                static_cast<double>(cam3d.position.z)
            };
            cam.vup = vec3 {
                static_cast<double>(cam3d.up.x),
                static_cast<double>(cam3d.up.y),
                static_cast<double>(cam3d.up.z)
            };
            cam.vfov = cam3d.fovy;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode3D(cam3d);
        world.draw();
        EndMode3D();

        if (cam.rendering() || cam.complete()) {
            for (int y = 0; y < dims.height; y += 1) {
                for (int x = 0; x < dims.width; x += 1) {
                    auto &px = bmp->pixel_at(x, y);
                    if (px.a) {
                        Color color { px.r, px.g, px.b, px.a };
                        DrawPixel(x, y, color);
                    }
                }
            }
        }

        int font_size = 20;
        if (cam.rendering() && !cam.complete()) {
            DrawText("Rendering...", 10, dims.height - 10 - font_size, font_size, BLACK);
            DrawText("Rendering...", 11, dims.height - 9 - font_size, font_size, RAYWHITE);
        }

        if (cam.complete()) {
            DrawText("DONE", 10, dims.height - 10 - font_size, font_size, BLACK);
            DrawText("DONE", 11, dims.height - 9 - font_size, font_size, RAYWHITE);
        }

        DrawFPS(10, 10);

        EndDrawing();
    }

    cam.cancel();
    if (future.valid()) {
        future.wait();
    }
    CloseWindow();
}
