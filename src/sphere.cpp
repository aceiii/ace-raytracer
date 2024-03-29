#include "sphere.h"
#include "material.h"

#include <spdlog/fmt/bundled/core.h>

sphere::sphere(point3 _center, double _radius, shared_ptr<material> _material)
    :center1(_center), radius(_radius), mat(_material)
{
    auto rvec = vec3(radius, radius, radius);
    bbox = aabb(center1 - rvec, center1 + rvec);

    spdlog::trace("Creating sphere at ({:0.2f},{:0.2f},{:0.2f}) with radius {:0.2f}", center1.x(), center1.y(), center1.z(), radius);
    spdlog::trace("Creating bounding box from ({:0.2f},{:0.2f},{:0.2f}) to ({:0.2f},{:0.2f},{:0.2f})", bbox.x.min, bbox.y.min, bbox.z.min, bbox.x.max, bbox.y.max, bbox.z.max);
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
    get_sphere_uv(outward_normal, rec.u, rec.v);
    rec.mat = mat;

    return true;
}

aabb sphere::bounding_box() const {
    return bbox;
}

void sphere::get_sphere_uv(const point3& p, double& u, double& v) {
    // p: a given point on the sphere of radius one, centered at the origin
    // u: returned value [0, 1] of angle around the Y axis from X=-1
    // v: returned value [0, 1] of angle from Y=-1 to Y=+1
    //      <1 0 0> yields <0.50 0.50>      <-1  0  0> yields <0.00 0.50>
    //      <0 1 0> yields <0.50 1.00>      < 0 -1  0> yields <0.50 0.00>
    //      <0 0 1> yields <0.25 0.50>      < 0  0 -1> yields <0.75 0.50>

    auto theta = acos(-p.y());
    auto phi = atan2(-p.z(), p.x()) + pi;

    u = phi / (2 * pi);
    v = theta / pi;
}
