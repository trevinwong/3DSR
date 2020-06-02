#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../ext/stb_image.h"

Texture::Texture(std::string_view path)
{
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load(path.data(), &width, &height, &channels, 0);
    
}
