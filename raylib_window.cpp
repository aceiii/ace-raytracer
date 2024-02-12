#include "raylib_window.h"

#include <raylib.h>
#include <raygui.h>

namespace {
    tf::Taskflow taskflow;
    tf::Future<void> future;

    Camera3D cam3d;
}

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

inline Vector3 toRaylibVector3(const vec3& vec) {
    return Vector3 {
        static_cast<float>(vec.x()),
        static_cast<float>(vec.y()),
        static_cast<float>(vec.z())
    };
}

inline vec3 fromRaylibVector3(const Vector3& vec) {
    return vec3 {
        static_cast<double>(vec.x),
        static_cast<double>(vec.y),
        static_cast<double>(vec.z)
    };
}

inline Color toRaylibColor(const pixel& px) {
    return Color{ px.r, px.g, px.b, px.a };
}

bool update(tf::Executor &executor, camera &cam, const hittable_list& world, std::shared_ptr<bitmap> bmp) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        return true;
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
        cam.lookat = fromRaylibVector3(cam3d.target);
        cam.lookfrom = fromRaylibVector3(cam3d.position);
        cam.vup = fromRaylibVector3(cam3d.up);
        cam.vfov = cam3d.fovy;
    }

    return false;
}

inline void draw_rendered_bmp(std::shared_ptr<bitmap> bmp) {
    for (int y = 0; y < GetScreenHeight(); y += 1) {
        for (int x = 0; x < GetScreenWidth(); x += 1) {
            auto &px = bmp->pixel_at(x, y);
            if (px.a) {
                DrawPixel(x, y, toRaylibColor(px));
            }
        }
    }
}

void draw(tf::Executor &executor, camera &cam, const hittable_list& world, std::shared_ptr<bitmap> bmp) {
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(cam3d);
    world.draw();
    EndMode3D();

    if (cam.rendering() || cam.complete()) {
        draw_rendered_bmp(bmp);
    }

    auto status_text = "Idle";
    if (cam.rendering() && !cam.complete()) {
        status_text = "Rendering...";
    } else if (cam.complete()) {
        status_text = "Done";
    }
    GuiStatusBar({ 0, (float)GetScreenHeight() - 32, (float)GetScreenWidth(), 32 }, status_text);

    DrawFPS(10, 10);

    EndDrawing();
}

void raylib_window::run(camera& cam, const hittable_list& world, std::shared_ptr<bitmap> bmp) {
    set_logging_level(log_level);

    tf::Executor executor(num_threads);

    auto start_render = taskflow.emplace([&world, &cam, bmp](tf::Subflow subflow) {
        cam.render(world, subflow, bmp);
    });

    dimensions dims = cam.get_image_dimensions();
    InitWindow(dims.width, dims.height, "Ace Raytracer!");

    cam3d.fovy = cam.vfov;
    cam3d.position = toRaylibVector3(cam.lookfrom);
    cam3d.target = toRaylibVector3(cam.lookat);
    cam3d.up = toRaylibVector3(cam.vup);
    cam3d.projection = CAMERA_PERSPECTIVE;

    bool forceSquaredChecked = true;

    while (!WindowShouldClose()) {
        if (update(executor, cam, world, bmp)) {
            break;
        }
        draw(executor, cam, world, bmp);
    }

    cam.cancel();
    if (future.valid()) {
        future.wait();
    }
    CloseWindow();
}
