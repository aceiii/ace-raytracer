#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
public:
    sphere(point3 _center, double _radius, shared_ptr<material> _material);

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override;
    void draw() const override;

private:
    point3 center;
    double radius;
    shared_ptr<material> mat;
};

#endif// __SPHERE_H__
