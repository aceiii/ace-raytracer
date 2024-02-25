#ifndef __RAYLIB_WINDOW_H__
#define __RAYLIB_WINDOW_H__

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "bitmap.h"

#include <memory>
#include <thread>

class raylib_window {
public:
    int num_threads = std::thread::hardware_concurrency();
    std::string log_level = "info";

    void run(camera& cam, const hittable_list& world, std::shared_ptr<bitmap> bmp);
};

struct draw_options {
    bool enable_debug;
};

#endif//__RAYLIB_WINDOW_H__
