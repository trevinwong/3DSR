#pragma once
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include <vector>
#include <string_view>
#include "polygon.h"
#include "../ext/tiny_obj_loader.h"

// Parses the .obj file into a list of vertices.
// Those vertices will have attributes in them populated by the rasterizer, which should technically be abstracted by the concept of a "vertex shader".
class Mesh
{
    public:
        Mesh() = default;
        Mesh(std::string_view path);
        void parse_obj(const tinyobj::attrib_t& attribs, const std::vector<tinyobj::shape_t>& shapes, const std::vector<tinyobj::material_t>& materials);
        void set_texture(const Texture& t);

        std::vector<Polygon> polygons;
        Texture texture;
};
