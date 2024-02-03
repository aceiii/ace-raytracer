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
    metal(const colour& a, double f): albedo(a), fuzz(f < 1 ? f : 1) {};

    bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override {
        auto reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz * random_unit_vector());
        attenuation = albedo;
        return dot(scattered.direction(), rec.normal) > 0;
    }

private:
    colour albedo;
    double fuzz;
};

class dieletric : public material {
public:
    dieletric(double index_of_refraction) : ir(index_of_refraction) {}

    bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override {
        attenuation = colour(1.0, 1.0, 1.0);
        double refraction_ratio = rec.front_face ? (1.0 /ir) : ir;

        vec3 unit_direction = unit_vector(r_in.direction());
        vec3 refracted = refract(unit_direction, rec.normal, refraction_ratio);

        scattered = ray(rec.p, refracted);
        return true;
    }

private:
    double ir; // index of refraction
};

#endif//__MATERIAL_H__
