#ifndef __HITTABLE_LIST_H__
#define __HITTABLE_LIST_H__

#include "hittable.h"
#include "interval.h"

#include <memory>
#include <vector>

class hittable_list : public hittable {
public:
    std::vector<std::shared_ptr<hittable>> objects;

    hittable_list();
    hittable_list(std::shared_ptr<hittable> object);

    void clear();
    void add(std::shared_ptr<hittable> object);
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override;
    inline aabb bounding_box() const override { return bbox; }

    void draw() const override;

private:
    aabb bbox;
};

#endif//__HITTABLE_LIST_H__
