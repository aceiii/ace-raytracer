#ifndef __BHV_H__
#define __BHV_H__

#include "rtweekend.h"
#include "hittable.h"
#include "hittable_list.h"

class bvh_node : public hittable {
public:
    bvh_node(const hittable_list& list) : bvh_node(list.objects, 0, list.objects.size()) {}

    bvh_node(const std::vector<shared_ptr<hittable>>& src_objects, size_t start, size_t end) {
        // To be implemented later...
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        if (!bbox.hit(r, ray_t)) {
            return false;
        }

        bool hit_left = left->hit(r, ray_t, rec);
        bool hit_right = right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

        return hit_left || hit_right;
    }

    aabb bounding_box() const override {
        return bbox;
    }

private:
    shared_ptr<hittable> left;
    shared_ptr<hittable> right;
    aabb bbox;
};

#endif//__BHV_H__
