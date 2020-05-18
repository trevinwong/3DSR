#pragma once
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include <vector>
#include <string_view>
#include "polygon.h"
#include "../lib/tiny_obj_loader.h"

// Parses the .obj file into a list of vertices.
class Mesh
{
    public:
        Mesh() = default;
        Mesh(std::string_view path);
        Mesh(std::string_view path, Texture* t);
        void parse_obj(const tinyobj::attrib_t& attribs, const std::vector<tinyobj::shape_t>& shapes, const std::vector<tinyobj::material_t>& materials);
        void set_texture(Texture* t);

        std::vector<Polygon> polygons;
        Texture* texture = nullptr;
};
