#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "rtweekend.h"

class hit_record;

class material {
public:
    virtual ~material() = default;

    virtual bool scatter(const ray& r_in, const hit_record& rec, colour &attenuation, ray& scattered) const = 0;
};

class lambertian : public material {
public:
    lambertian(const colour& a): albedo(a) {}

    bool scatter(const ray& r_in, const hit_record& rec, colour &attenuation, ray& scattered) const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        // prevent issues when normal and random vector from being opposites otherwise bad happens
        if (scatter_direction.near_zero()) {
            scatter_direction = rec.normal;
        }

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

private:
    colour albedo;
};

class metal : public material {
public:
    metal(const colour& a): albedo(a) {};

    bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scatterd) const override {
        auto reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scatterd = ray(rec.p, reflected);
        attenuation = albedo;
        return true;
    }

private:
    colour albedo;
};

#endif//__MATERIAL_H__
