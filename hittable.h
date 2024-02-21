#ifndef __HITTABLE_H__
#define __HITTABLE_H__

#include "ray.h"
#include "interval.h"
#include "aabb.h"

#include <memory>

class material;

class hit_record {
public:
    point3 p;
    vec3 normal;
    std::shared_ptr<material> mat;
    double t;
    double u;
    double v;
    bool front_face;

    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        // Sets the hit record normal vector
        // NOTE: the parameter `outward_normal` is assumed to have unit length

        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    virtual ~hittable() = default;

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
    virtual aabb bounding_box() const = 0;

    virtual void draw() const = 0;
};

class translate : public hittable {
public:
    translate(shared_ptr<hittable> p, const vec3& displacement)
        : object(p), offset(displacement)
    {
        bbox = object->bounding_box() + offset;
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // move ray backwards by offset
        ray offset_r(r.origin() - offset, r.direction(), r.time());

        // determine point of intersection
        if (!object->hit(offset_r, ray_t, rec)) {
            return false;
        }

        // moe intersection point forwards by offset
        rec.p += offset;
        return true;
    }

    aabb bounding_box() const override {
        return bbox;
    }

    void draw() const override {
        // object->draw();
    }

private:
    shared_ptr<hittable> object;
    vec3 offset;
    aabb bbox;
};

#endif// __HITTABLE_H__
