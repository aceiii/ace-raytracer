#include "sphere.h"
#include "material.h"

#include <raylib.h>
#include <spdlog/fmt/bundled/core.h>

sphere::sphere(point3 _center, double _radius, shared_ptr<material> _material)
    :center1(_center), radius(_radius), mat(_material)
{
    auto rvec = vec3(radius, radius, radius);
    bbox = aabb(center1 - rvec, center1 + rvec);
}

sphere::sphere(point3 _center1, point3 _center2, double _radius, shared_ptr<material> _material)
    : center1(_center1), radius(_radius), mat(_material), is_moving(true)
{
    auto rvec = vec3(radius, radius, radius);
    aabb box1(_center1 - rvec, _center1 + rvec);
    aabb box2(_center2 - rvec, _center2 + rvec);
    bbox = aabb(box1, box2);

    center_vec = _center2 - _center1;
}

bool sphere::hit(const ray& r, interval ray_t, hit_record& rec) const {
    point3 center0 = is_moving ? sphere::center(r.time()) : center1;
    vec3 oc = r.origin() - center0;
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
    vec3 outward_normal = (rec.p - center0) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat = mat;

    return true;
}

aabb sphere::bounding_box() const {
    return bbox;
}

void sphere::draw() const {
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

    Vector3 pos {
        static_cast<float>(bbox.x.min),
        static_cast<float>(bbox.y.min),
        static_cast<float>(bbox.z.min),
    };

    auto width = static_cast<float>(bbox.x.size());
    auto height = static_cast<float>(bbox.y.size());
    auto length = static_cast<float>(bbox.z.size());

    DrawCubeWires(pos, width, height, length, MAROON);
}
