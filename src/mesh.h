#pragma once
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include <vector>
#include <string_view>
#include "../lib/tiny_obj_loader.h"

class Mesh
{
    public:
        Mesh() = default;
        Mesh(std::string_view path);
        void parse_obj(const tinyobj::attrib_t& attribs, const std::vector<tinyobj::shape_t>& shapes, const std::vector<tinyobj::material_t>& materials);

        std::vector<vec3> local_coords;
        std::vector<vec2> texture_coords;
        std::vector<vec3> normals;
};
