#include "bvh.h"
#include "vec3.h"

#include <raylib.h>

void bvh_node::draw() const {
    left->draw();
    right->draw();

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
