#ifndef __BITMAP_H__
#define __BITMAP_H__

#include "pixel.h"

#include <spdlog/spdlog.h>
#include <stb_image_write.h>

class bitmap {
public:
    const int width;
    const int height;

    pixel* data;

    bitmap(int w, int h): width(w), height(h) {
        data = new pixel[width * height];
    }

    ~bitmap() {
        delete data;
    }

    pixel& pixel_at(int x, int y) {
        return data[(y * width) + x];
    }

    void write_to_file(std::string filename) {
        spdlog::info("Writing bitmap to file: {}", filename);

        const int channels = 4;

        auto img = stbi_write_png(filename.c_str(), width, height, channels, data, channels * width);
        if (!img) {
            spdlog::error("Failed to write file");
        } else {
            spdlog::info("Write to file complete.");
        }
    }
};


#endif//__BITMAP_H__
