#include "raylib_window.h"

#include <raylib.h>

void raylib_window::run(camera& cam, const hittable_list& world, std::shared_ptr<bitmap> bmp) {
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

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) {
            break;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode3D(cam3d);
        world.draw();
        EndMode3D();

        for (int y = 0; y < dims.height; y += 1) {
            for (int x = 0; x < dims.width; x += 1) {
                auto &px = bmp->pixel_at(x, y);
                if (px.a) {
                    Color color { px.r, px.g, px.b, px.a };
                    DrawPixel(x, y, color);
                }
            }
        }

        EndDrawing();
    }

    cam.cancel();
    CloseWindow();
}
