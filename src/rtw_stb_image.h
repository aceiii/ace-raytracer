#ifndef __RTW_IMAGE_H__
#define __RTW_IMAGE_H__

#include <stb_image.h>
#include <spdlog/spdlog.h>
#include <cstdlib>
#include <string>
#include <filesystem>

class rtw_image {
public:
    rtw_image() : data(nullptr) {}

    rtw_image(const char* image_filename) {

        auto filename = std::string(image_filename);
        auto imagedir = getenv("RTW_IMAGES");

        // Hunt for image file in some likely locations
        if (imagedir && load(std::string(imagedir) + "/" + image_filename)) { return; }
        if (load(filename)) { return ; }
        if (load("./images/" + filename)) { return; }
        if (load("../images/" + filename)) { return; }
        if (load("../../images/" + filename)) { return; }
        if (load("../../../images/" + filename)) { return; }
        if (load("../../../../images/" + filename)) { return; }
        if (load("../../../../../images/" + filename)) { return; }
        if (load("../../../../../../images/" + filename)) { return; }

        spdlog::error("Could not load image file '{}'", image_filename);
    }

    ~rtw_image() {
        // STBI_FREE(data);
        free(data);
    }

    bool load(const std::string filename) {
        auto n = bytes_per_pixel;
        auto filepath = std::filesystem::absolute(filename);
        data = stbi_load(filepath.c_str(), &image_width, &image_height, &n, bytes_per_pixel);
        bytes_per_scanline = image_width * bytes_per_pixel;

        if (data == nullptr) {
            spdlog::warn("Failed to load file at: {}", filepath.string());
            spdlog::warn("Failure reason: {}", stbi_failure_reason());
            return false;
        }

        spdlog::debug("Loaded file at: {}", filename);
        return true;
    }

    int width() const {
        return (data == nullptr) ? 0 : image_width;
    }

    int height() const {
        return (data == nullptr) ? 0 : image_height;
    }

    const unsigned char* pixel_data(int x, int y) const {
        // return the address of the 3 bytes of pixel at (x,y) otherwise magenta if no data
        static unsigned char magenta[] = { 255, 0, 255 };
        if (data == nullptr) {
            return magenta;
        }

        x = clamp(x, 0, image_width);
        y = clamp(y, 0, image_height);

        return data + (y * bytes_per_scanline) + (x * bytes_per_pixel);
    }

private:
    const int bytes_per_pixel = 3;
    unsigned char* data;
    int image_width, image_height;
    int bytes_per_scanline;

    static int clamp(int x, int low, int high) {
        if (x < low) {
            return low;
        }
        if (x < high) {
            return x;
        }
        return high -1;
    }
};

#endif//__RTW_IMAGE_H__
