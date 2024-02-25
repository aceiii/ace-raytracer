#include "rtweekend.h"
#include "bvh.h"
#include "constant_medium.h"
#include "hittable.h"
#include "material.h"
#include "quad.h"
#include "ray.h"
#include "sphere.h"
#include "vec3.h"
#include "raylib_window.h"

#include <raylib.h>
#include <rlgl.h>

void hittable_list::draw(const draw_options& options) const {
    for (const auto& object : objects) {
        object->draw(options);
    }
}

void bvh_node::draw(const draw_options& options) const {
    left->draw(options);
    right->draw(options);

    if (options.enable_debug) {
        auto width = static_cast<float>(bbox.x.size());
        auto height = static_cast<float>(bbox.y.size());
        auto length = static_cast<float>(bbox.z.size());

        Vector3 pos {
            static_cast<float>(bbox.x.min) + (width / 2),
            static_cast<float>(bbox.y.min) + (height / 2),
            static_cast<float>(bbox.z.min) + (length / 2),
        };

        DrawCubeWires(pos, width, height, length, RAYWHITE);
    }
}

void quad::draw(const draw_options& options) const {
    auto mat_col = mat->get_colour();

    Color col {
        static_cast<uint8_t>(mat_col.x() * 255.999),
        static_cast<uint8_t>(mat_col.y() * 255.999),
        static_cast<uint8_t>(mat_col.z() * 255.999),
        255,
    };

    auto a = Q;
    auto b = Q + u;
    auto c = Q + v;
    auto d = Q + u + v;

    Vector3 points[4] = {
        { static_cast<float>(a.x()), static_cast<float>(a.y()), static_cast<float>(a.z()) },
        { static_cast<float>(b.x()), static_cast<float>(b.y()), static_cast<float>(b.z()) },
        { static_cast<float>(c.x()), static_cast<float>(c.y()), static_cast<float>(c.z()) },
        { static_cast<float>(d.x()), static_cast<float>(d.y()), static_cast<float>(d.z()) },
    };

    DrawTriangleStrip3D(points, 4, col);
}

void sphere::draw(const draw_options& options) const {
    Vector3 ctr {
        static_cast<float>(center1.x()),
        static_cast<float>(center1.y()),
        static_cast<float>(center1. z())
    };

    auto c = mat->get_colour();
    Color col {
        static_cast<uint8_t>(c.x() * 255.999),
        static_cast<uint8_t>(c.y() * 255.999),
        static_cast<uint8_t>(c.z() * 255.9999),
        255,
    };

    int rings = 10;
    int slices = 10;

    if (radius > 99) {
        rings = 20;
        slices = 20;
    }

    if (radius > 499) {
        rings = 40;
        slices = 40;
    }

    DrawSphereEx(ctr, radius, rings, slices, col);

    if (options.enable_debug) {
        Vector3 size {
            static_cast<float>(bbox.x.size()),
            static_cast<float>(bbox.y.size()),
            static_cast<float>(bbox.z.size()),
        };

        spdlog::trace("Drawing bbox at ({:0.2f},{:0.2f},{:0.2f}) with sides of size ({:0.2f},{:0.2f},{:0.2f})", ctr.x, ctr.y, ctr.z, size.x, size.y, size.z);

        DrawCubeWiresV(ctr, size, MAROON);
    }
}

void constant_medium::draw(const draw_options& options) const {
    boundary->draw(options);
}

void translate::draw(const draw_options& options) const {
    rlPushMatrix();
    rlTranslatef(
        static_cast<float>(offset.x()),
        static_cast<float>(offset.y()),
        static_cast<float>(offset.z())
    );
    object->draw(options);
    rlPopMatrix();
}

void rotate_y::draw(const draw_options& options) const {
    rlPushMatrix();
    rlRotatef(angle, 0, 1, 0);
    object->draw(options);
    rlPopMatrix();
}
