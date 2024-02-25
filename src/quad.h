#ifndef __QUAD_H__
#define __QUAD_H__

#include "rtweekend.h"
#include "hittable.h"
#include "hittable_list.h"

class quad : public hittable {
public:
    quad(const point3& _Q, const vec3& _u, const vec3& _v, shared_ptr<material> m)
        : Q(_Q), u(_u), v(_v), mat(m)
    {
        auto n = cross(u, v);
        normal = unit_vector(n);
        D = dot(normal, Q);
        w = n / dot(n, n);

        set_bounding_box();
    }

    virtual void set_bounding_box() {
        bbox = aabb(Q, Q + u + v).pad();
    }

    aabb bounding_box() const override {
        return bbox;
    }

    bool hit(const ray&r, interval ray_t, hit_record& rec) const override {
        auto denom = dot(normal, r.direction());

        // no hit if ray is parallel to plane
        if (fabs(denom) < 1e-8) {
            return false;
        }

        // return false if hit point parameter t is outside of ray interval
        auto t = (D - dot(normal, r.origin())) / denom;
        if (!ray_t.contains(t)) {
            return false;
        }

        // determine if hit point lies within planar shape using plane coords
        auto intersection = r.at(t);
        vec3 planar_hitpt_vector = intersection - Q;
        auto alpha = dot(w, cross(planar_hitpt_vector, v));
        auto beta = dot(w, cross(u, planar_hitpt_vector));

        if (!is_interior(alpha, beta, rec)) {
            return false;
        }

        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, normal);

        return true;
    }

    virtual bool is_interior(double a, double b, hit_record& rec) const {
        // given the point in the plane coordinates, return false if it is outside the
        // primitive, otherwise set the hit record uv coords and return true

        if ((a < 0) || (1 < a) || (b < 0) || (1 < b)) {
            return false;
        }

        rec.u = a;
        rec.v = b;
        return true;
    }

    void draw(const draw_options& options) const override;

private:
    point3 Q;
    vec3 u, v;
    shared_ptr<material> mat;
    aabb bbox;
    vec3 normal;
    double D;
    vec3 w;
};

inline shared_ptr<hittable_list> box(const point3& a, const point3& b, shared_ptr<material> mat) {
    auto sides = make_shared<hittable_list>();

    auto min = point3(fmin(a.x(), b.x()), fmin(a.y(), b.y()), fmin(a.z(), b.z()));
    auto max = point3(fmax(a.x(), b.x()), fmax(a.y(), b.y()), fmax(a.z(), b.z()));

    auto dx = vec3(max.x() - min.x(), 0, 0);
    auto dy = vec3(0, max.y() - min.y(), 0);
    auto dz = vec3(0, 0, max.z() - min.z());

    sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()),  dx,  dy, mat)); // front
    sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz,  dy, mat)); // right
    sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx,  dy, mat)); // back
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dz,  dy, mat)); // left
    sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()),  dx, -dz, mat)); // top
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dx,  dz, mat)); // bottom

    return sides;
}

#endif//__QUAD_H__
