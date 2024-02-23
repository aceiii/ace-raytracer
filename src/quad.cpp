#include "quad.h"
#include "material.h"

#include <raylib.h>

void quad::draw() const {
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
