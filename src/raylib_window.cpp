#include "raylib_window.h"

#include <raylib.h>
#include <raygui.h>

namespace {
    tf::Taskflow taskflow;
    tf::Future<void> future;

    Camera3D cam3d;
    Texture2D renderedTexture;

    float samples;
    float max_depth;

    bool show_panel = true;
    bool enable_move = false;
    bool enable_debug = false;
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

inline bool update(tf::Executor &executor, camera &cam, const hittable_list& world, std::shared_ptr<bitmap> bmp) {
    if (IsKeyPressed(KEY_SPACE)) {
        show_panel = !show_panel;
        enable_move = !show_panel && !cam.rendering();
    }

    if (!cam.rendering() && enable_move) {
        UpdateCamera(&cam3d, CAMERA_FIRST_PERSON);
    }

    UpdateTexture(renderedTexture, bmp->data);

    return false;
}

inline void draw(tf::Executor &executor, camera &cam, const hittable_list& world, std::shared_ptr<bitmap> bmp) {
    auto restart_render = [&, bmp]() {
        bmp->clear();
        cam.cancel();
        if (future.valid()) {
            future.wait();
        }
        future = executor.run(taskflow);
    };

    float screen_width = GetScreenWidth();
    float screen_height = GetScreenHeight();

    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(cam3d);

    draw_options options = {
        enable_debug,
    };
    world.draw(options);
    EndMode3D();

    DrawTexture(renderedTexture, 0, 0, WHITE);

    auto status_text = "Idle";
    auto button_text = "Render";
    if (cam.rendering() && !cam.complete()) {
        status_text = "Rendering...";
        button_text = "Cancel Rendering";
    } else if (cam.complete()) {
        status_text = "Done";
    }

    int margin = 10;
    int status_bar_height = 20;
    int panel_width = 280;
    int panel_x = screen_width - panel_width - margin;
    int panel_y = margin;
    int panel_height = screen_height - margin -margin - status_bar_height;
    int padding = 16;

    if (show_panel) {
        DrawRectangle(panel_x, panel_y, panel_width, panel_height , Fade(RAYWHITE, 0.8));
        BeginScissorMode(panel_x, panel_y, panel_width, panel_height);

        int item_x = panel_x + padding;
        int item_y = panel_y + padding;
        int max_item_width = panel_width - padding - padding;

        if (cam.rendering() && !cam.complete()) {
            GuiLock();
            GuiSetState(STATE_DISABLED);
        }

        GuiCheckBox(Rectangle{ static_cast<float>(item_x), static_cast<float>(item_y), 16, 16 }, "Enable Debug Drawing", &enable_debug);
        item_y += 20 + 16;

        DrawText(TextFormat("Samples: %d", (int)samples), item_x, item_y, 10, BLACK);
        item_y += 10 + 8;
        GuiSlider({ (float)item_x + 16, (float)item_y, (float)max_item_width - 32, 20 }, "1", "255", (float*)&samples, 1, 255);
        item_y += 20 + 16;
        DrawText(TextFormat("Max Depth: %d", (int)max_depth), item_x, item_y, 10, BLACK);
        item_y += 10 + 8;
        GuiSlider({ (float)item_x + 16, (float)item_y, (float)max_item_width - 32, 20 }, "1", "80", (float*)&max_depth, 1, 80);

        GuiSetState(STATE_NORMAL);
        GuiUnlock();

        if (GuiButton({ (float)panel_x + padding, (float)panel_height - 32, (float)panel_width - 32, 32 }, button_text)) {
            spdlog::trace("Render clicked!");

            if (cam.rendering() && !cam.complete()) {
                cam.cancel();
            } else {
                restart_render();
            }
        }

        EndScissorMode();
    }

    if (GuiButton({ (float)panel_x + panel_width - 20 - 4, (float)panel_y + 4, 20, 20 }, show_panel ? "X" : "O")) {
        spdlog::trace("Panel Close Clicked");
        show_panel = !show_panel;
        enable_move = !show_panel && !cam.rendering();
    }

    GuiStatusBar({ 0, screen_height - status_bar_height, screen_width, (float)status_bar_height }, status_text);

    DrawFPS(10, 10);

    EndDrawing();
}

void raylib_window::run(camera& cam, const hittable_list& world, std::shared_ptr<bitmap> bmp) {
    set_logging_level(log_level);

    tf::Executor executor(num_threads);

    taskflow.emplace([&world, &cam, bmp](tf::Subflow subflow) {
        cam.lookat = fromRaylibVector3(cam3d.target);
        cam.lookfrom = fromRaylibVector3(cam3d.position);
        cam.vup = fromRaylibVector3(cam3d.up);
        cam.vfov = cam3d.fovy;
        cam.samples_per_pixel = (int)samples;
        cam.max_depth = (int)max_depth;
        cam.init();
        cam.render(world, subflow, bmp);
    });

    dimensions dims = cam.get_image_dimensions();
    InitWindow(dims.width, dims.height, "Ace Raytracer!");
    SetExitKey(KEY_ESCAPE);

    Image imgBlank = GenImageColor(dims.width, dims.height, BLANK);
    renderedTexture = LoadTextureFromImage(imgBlank);

    cam3d.fovy = cam.vfov;
    cam3d.position = toRaylibVector3(cam.lookfrom);
    cam3d.target = toRaylibVector3(cam.lookat);
    cam3d.up = toRaylibVector3(cam.vup);
    cam3d.projection = CAMERA_PERSPECTIVE;

    samples = cam.samples_per_pixel;
    max_depth = cam.max_depth;

    SetTargetFPS(60);

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
