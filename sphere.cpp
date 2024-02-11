#include "sphere.h"
#include "material.h"

#include <raylib.h>
#include <spdlog/fmt/bundled/core.h>

sphere::sphere(point3 _center, double _radius, shared_ptr<material> _material):
    center{_center}, radius{_radius}, mat{_material} {}

bool sphere::hit(const ray& r, interval ray_t, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;

    auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0) {
        return false;
    }

    auto sqrtd = sqrt(discriminant);

    // Find nearest root that lies in acceptable range
    auto root = (-half_b - sqrtd) / a;
    if (!ray_t.surrounds(root)) {
        root = (-half_b + sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            return false;
        }
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat = mat;

    return true;
}

void sphere::draw() const {
    Vector3 ctr {
        static_cast<float>(center.x()),
        static_cast<float>(center.y()),
        static_cast<float>(center. z())
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
}
