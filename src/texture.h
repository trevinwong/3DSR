#pragma once
#include <string_view>

class Texture
{
    public:
        Texture() = default;
        Texture(std::string_view path);
        int width = 0;
        int height = 0;
        int channels = 0;
        unsigned char* data = nullptr;
};
