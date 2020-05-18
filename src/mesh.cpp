#define TINYOBJLOADER_IMPLEMENTATION
#include "vertex.h"
#include "mesh.h"

Mesh::Mesh(std::string_view path)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;  

    // LoadObj now only passes in 1 std::string for error handling for release v1.0.6.
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.data());

    if (!err.empty()) {
      std::cerr << "Error: " << err << std::endl;
    }

    if (!ret) {
      exit(1);
    }

    parse_obj(attrib, shapes, materials);
}

Mesh::Mesh(std::string_view path, Texture* t)
    : Mesh(path)
{ 
    texture = t;
}

void Mesh::parse_obj(const tinyobj::attrib_t& attrib, const std::vector<tinyobj::shape_t>& shapes, const std::vector<tinyobj::material_t>& materials)
{
    for (size_t s = 0; s < shapes.size(); s++)
    {
        size_t f_index_begin = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            Polygon polygon;
            int num_vertices = shapes[s].mesh.num_face_vertices[f];

            // All vertices are stored in counter-clockwise order by default.
            for (int v = 0; v < num_vertices; v++)
            {
                Vertex vertex;

                tinyobj::index_t idx = shapes[s].mesh.indices[f_index_begin + v]; 

                tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index + 2];
                vertex.local_coords = vec3(vx, vy, vz);

                tinyobj::real_t nx = attrib.normals[3*idx.normal_index + 0];
                tinyobj::real_t ny = attrib.normals[3*idx.normal_index + 1];
                tinyobj::real_t nz = attrib.normals[3*idx.normal_index + 2];
                vertex.normal =  vec3(nx, ny, nz);

                tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
                tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];
                vertex.texture_coords = vec2(tx, ty);

                polygon.vertices.push_back(vertex);
            }

            polygons.push_back(polygon);
            f_index_begin += num_vertices;
        }
    }

}

void Mesh::set_texture(Texture* t)
{
    texture = t;
}
