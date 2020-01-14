#pragma once
#include <stdint.h>
#include <utility>

class Frame
{
    public:
        int w = 0;
        int h = 0;
        uint32_t* buffer = nullptr;

        Frame() = default;
        Frame(int width, int height);
        Frame(const Frame& other);
        Frame& operator=(const Frame& other);
        Frame(Frame&& other) noexcept;
        Frame& operator=(Frame&& other) noexcept;
        ~Frame();
        void fill_frame_with_color(uint32_t color);
        void set_pixel(int x, int y, uint32_t color);
        void flip_image_on_x_axis();
    private:
        void copy(const Frame& other);
};
