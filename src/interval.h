#ifndef __INTERVAL_H__
#define __INTERVAL_H__

#include "rtweekend.h"

class interval {
public:
    double min, max;

    inline interval(): min(+infinity), max(-infinity) {} // Default interval is empty

    inline interval(double _min, double _max): min(_min), max(_max) {}

    inline interval(const interval& a, const interval& b)
        : min(fmin(a.min, b.min)), max(fmax(a.max, b.max)) {}

    inline bool contains(double x) const {
        return min <= x && x <= max;
    }

    inline bool surrounds(double x) const {
        return min < x && x < max;
    }

    inline double clamp(double x) const {
        if (x < min) {
            return min;
        }
        if (x > max) {
            return max;
        }
        return x;
    }

    inline double size() const {
        return max - min;
    }

    inline interval expand(double delta) const {
        auto padding = delta / 2;
        return interval(min - padding, max + padding);
    }

    static const interval empty, universe;
};

inline const interval interval::empty = interval(+infinity, -infinity);
inline const interval interval::universe = interval(-infinity, +infinity);

inline interval operator+(const interval& ival, double displacement) {
    return interval(ival.min + displacement, ival.max + displacement);
}

inline interval operator+(double displacement, const interval& ival) {
    return ival + displacement;
}

#endif//__INTERVAL_H__
