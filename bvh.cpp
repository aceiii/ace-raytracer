#include "bvh.h"
#include "vec3.h"

#include <raylib.h>

void bvh_node::draw() const {
    left->draw();
    right->draw();

    Vector3 pos {
        static_cast<float>(bbox.x.min),
        static_cast<float>(bbox.y.min),
        static_cast<float>(bbox.z.min),
    };

    auto width = static_cast<float>(bbox.x.size());
    auto height = static_cast<float>(bbox.y.size());
    auto length = static_cast<float>(bbox.z.size());

    DrawCubeWires(pos, width, height, length, RAYWHITE);
}
