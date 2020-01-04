#include "frame.h"

Frame::Frame(int width, int height)
    : w(width), h(height)
{
    buffer = new uint32_t[(w * h) * sizeof(uint32_t)];
}

Frame::Frame(const Frame& other)
{
    copy(other);
}

Frame& Frame::operator=(const Frame& other)
{
    if (this != &other)
    {
        copy(other);
    }
    return *this;
}

Frame::Frame(Frame&& other) noexcept
{
    buffer = other.buffer;
    other.buffer = nullptr;
}

Frame& Frame::operator=(Frame&& other) noexcept
{
    if (this != &other)
    {
        buffer = other.buffer;
        other.buffer = nullptr;
    }
    return *this;
}

Frame::~Frame()
{
    delete[] buffer;
}

void Frame::set_pixel(int x, int y, uint32_t color)
{
    // The buffer contains each row of the frame in a linear order.
    // We point to the first pixel at (0, 0), select the row using (w * y), then select our column using x.
    if (x >= w) return;
    if (y >= h) return;
    uint32_t* ptr = buffer + (w * y) + x;
    *ptr = color;
}

void Frame::flip_image_on_x_axis()
{
    for (int i = 0; i < (w/2)*h; i++)
    {
        std::swap(buffer[i], buffer[(w*h)-1-i]);
    }
}

void Frame::copy(const Frame& other)
{
    buffer = new uint32_t[(other.w * other.h) * sizeof(uint32_t)];
}
