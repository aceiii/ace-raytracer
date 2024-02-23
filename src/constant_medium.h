#ifndef __CONSTANT_MEDIUM_H__
#define __CONSTANT_MEDIUM_H__

#include "rtweekend.h"
#include "hittable.h"
#include "material.h"
#include "texture.h"

#include <spdlog/spdlog.h>

class constant_medium : public hittable {
public:
    constant_medium(shared_ptr<hittable> b, double d, shared_ptr<texture> a)
        : boundary(b), neg_inv_density(-1/d), phase_function(make_shared<isotropic>(a))
    {}

    constant_medium(shared_ptr<hittable> b, double d, colour c)
        : boundary(b), neg_inv_density(-1/d), phase_function(make_shared<isotropic>(c))
    {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // print samples when debugging when enableDebug set to true
        const bool enableDebug = false;
        const bool debugging = enableDebug && random_double() < 0.00001;

        hit_record rec1, rec2;

        if (!boundary->hit(r, interval::universe, rec1)) {
            return false;
        }

        if (!boundary->hit(r, interval(rec1.t + 0.0001, infinity), rec2)) {
            return false;
        }

        if (debugging) {
            spdlog::trace("ray_tmin = {:0.3f}, ray_tmax = {:0.3f}", rec1.t, rec2.t);
        }

        if (rec1.t < ray_t.min) {
            rec1.t = ray_t.min;
        }
        if (rec2.t > ray_t.max) {
            rec2.t = ray_t.max;
        }

        if (rec1.t >= rec2.t) {
            return false;
        }

        if (rec1.t < 0) {
            rec1.t = 0;
        }

        auto ray_length = r.direction().length();
        auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
        auto hit_distance = neg_inv_density * log(random_double());

        if (hit_distance > distance_inside_boundary) {
            return false;
        }

        rec.t = rec1.t + hit_distance / ray_length;
        rec.p = r.at(rec.t);

        if (debugging) {
            spdlog::trace("hit_distance = {:0.3f}, rec.t={:0.3f}, rec.p = ({:.2f},{:.2f},{:.2f})", hit_distance, rec.t, rec.p.x(), rec.p.y(), rec.p.z());
        }

        rec.normal = vec3(1, 0, 0); // arbitrary
        rec.front_face = true; // also arbitrary
        rec.mat = phase_function;

        return true;
    }

    aabb bounding_box() const override {
        return boundary->bounding_box();
    }

    void draw() const override {
        // object->draw();
    }

private:
    shared_ptr<hittable> boundary;
    double neg_inv_density;
    shared_ptr<material> phase_function;
};

#endif//__CONSTANT_MEDIUM_H__
