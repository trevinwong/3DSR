#pragma once
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "face.h"
#include <memory>
#include <vector>
#include <string_view>
#include "../ext/tiny_obj_loader.h"

class Mesh
{
    private:
        std::vector<Face> faces;
        std::shared_ptr<Texture> texture = nullptr;

    private: 
        void parse_obj(const tinyobj::attrib_t& attribs, const std::vector<tinyobj::shape_t>& shapes, const std::vector<tinyobj::material_t>& materials);
        
    public:
        Mesh() = default;
        Mesh(std::string_view path);
        Mesh(std::string_view path, std::shared_ptr<Texture>& t);

        void setTexture(std::shared_ptr<Texture>& t);
        const std::shared_ptr<Texture>& getTexture() const; 

        void setFaces(std::vector<Face>& f);
        // TODO: Make const, with refactor of rasterizer
        std::vector<Face>& getFaces();
};
