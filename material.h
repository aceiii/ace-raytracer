#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "rtweekend.h"
#include "colour.h"
#include "texture.h"

#include <algorithm>

class hit_record;

class material {
public:
    virtual ~material() = default;

    virtual colour emitted(double u, double v, const point3& p) const {
        return colour(0, 0, 0);
    }

    virtual bool scatter(const ray& r_in, const hit_record& rec, colour &attenuation, ray& scattered) const = 0;

    // for drawing to viewport
    virtual colour get_colour() const = 0;
};

class lambertian : public material {
public:
    lambertian(const colour& a) : albedo(make_shared<solid_colour>(a)), col(a) {}
    lambertian(shared_ptr<texture> a) : albedo(a) {
        // sample colours from texture to generate a single averaged colour
        int samples = 1000;
        point3 p(0, 0, 0);
        for (int i = 0; i < samples; i += 1) {
            col = col + albedo->value(random_double(0, 1), random_double(0, 1), p);
        }
        col = col / samples;
    }

    bool scatter(const ray& r_in, const hit_record& rec, colour &attenuation, ray& scattered) const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        // prevent issues when normal and random vector from being opposites otherwise bad happens
        if (scatter_direction.near_zero()) {
            scatter_direction = rec.normal;
        }

        scattered = ray(rec.p, scatter_direction, r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

    colour get_colour() const override {
        return col;
    }

private:
    shared_ptr<texture> albedo;
    colour col;
};

class metal : public material {
public:
    metal(const colour& a, double f): albedo(a), fuzz(f < 1 ? f : 1) {};

    bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override {
        auto reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time());
        attenuation = albedo;
        return dot(scattered.direction(), rec.normal) > 0;
    }

    colour get_colour() const override {
        return albedo;
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
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double()) {
            direction = reflect(unit_direction, rec.normal);
        } else {
            direction = refract(unit_direction, rec.normal, refraction_ratio);
        }

        scattered = ray(rec.p, direction, r_in.time());
        return true;
    }

    colour get_colour() const override {
        return colour { 0.5, 0.4, 0.6 };
    }

private:
    double ir; // index of refraction

    static double reflectance(double cosine, double ref_idx) {
        // use Schlick's approximation for reflectance
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow(1 - cosine, 5);
    }
};

class diffuse_light : public material {
public:
    diffuse_light(shared_ptr<texture> a) : emit(a) {
        set_colour();
    }

    diffuse_light(colour c) : emit(make_shared<solid_colour>(c)), col(c) {}

    bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override {
        return false;
    }

    colour emitted(double u, double v, const point3& p) const override {
        return emit->value(u, v, p);
    }

    colour get_colour() const override {
        return col;
    }

private:
    shared_ptr<texture> emit;
    colour col;

    void set_colour() {
        // sample colours from texture to generate a single averaged colour
        int samples = 1000;
        point3 p(0, 0, 0);
        for (int i = 0; i < samples; i += 1) {
            col = col + emit->value(random_double(0, 1), random_double(0, 1), p);
        }
        col = colour(
            std::clamp(col.x() / static_cast<double>(samples), 0.0, 1.0),
            std::clamp(col.y() / static_cast<double>(samples), 0.0, 1.0),
            std::clamp(col.z() / static_cast<double>(samples), 0.0, 1.0)
        );
    }
};

#endif//__MATERIAL_H__
