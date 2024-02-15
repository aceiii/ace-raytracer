#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "hittable.h"
#include "vec3.h"
#include "aabb.h"

class sphere : public hittable {
public:
    // stationary
    sphere(point3 _center, double _radius, shared_ptr<material> _material);

    // moving sphere
    sphere(point3 _center1, point3 _center2, double _radius, shared_ptr<material> _material);

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override;
    aabb bounding_box() const override;
    void draw() const override;

private:
    point3 center1;
    double radius;
    shared_ptr<material> mat;
    bool is_moving;
    vec3 center_vec;
    aabb bbox;

    inline point3 center(double time) const {
        return center1 + time * center_vec;
    }
};

#endif// __SPHERE_H__
