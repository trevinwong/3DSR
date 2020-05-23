#define TINYOBJLOADER_IMPLEMENTATION
#include "vertex.h"
#include "mesh.h"

// All vertices are stored in counter-clockwise order by default.
void Mesh::parse_obj(const tinyobj::attrib_t& attrib, const std::vector<tinyobj::shape_t>& shapes, const std::vector<tinyobj::material_t>& materials)
{
    int shapeAmt = shapes.size();
    
    for (size_t s = 0; s < shapeAmt; s++)
    {
        int faceAmt = shapes[s].mesh.num_face_vertices.size();
        size_t startVertexIdxOfFace = 0; // Vertices are not stored per face, but rather all in an array

        for (size_t f = 0; f < faceAmt; f++)
        {
            int vertexAmt = shapes[s].mesh.num_face_vertices[f];
            Face face;
            
            for (int vertexNum = 0; vertexNum < vertexAmt; vertexNum++)
            {
                Vertex vertex;

                tinyobj::index_t idx = shapes[s].mesh.indices[startVertexIdxOfFace + vertexNum]; 

                tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index + 2];
                // DEPRECATED
                vertex.local_coords = vec3(vx, vy, vz);
                vertex.position = vec4(vx, vy, vz, 1);

                tinyobj::real_t nx = attrib.normals[3*idx.normal_index + 0];
                tinyobj::real_t ny = attrib.normals[3*idx.normal_index + 1];
                tinyobj::real_t nz = attrib.normals[3*idx.normal_index + 2];
                vertex.normal =  vec4(nx, ny, nz, 0);

                tinyobj::real_t u = attrib.texcoords[2*idx.texcoord_index+0];
                tinyobj::real_t v = attrib.texcoords[2*idx.texcoord_index+1];
                // DEPRECATED
                vertex.texture_coords = vec2(u, v);
                vertex.uv = vec2(u,v);

                face.vertices.push_back(vertex);
            }

            faces.push_back(face);
            startVertexIdxOfFace += vertexAmt;
        }
    }
}


Mesh::Mesh(std::string_view path)
{
    tinyobj::attrib_t                attrib;
    std::vector<tinyobj::shape_t>    shapes;
    std::vector<tinyobj::material_t> materials;
    std::string                      err;  

    // LoadObj now only passes in 1 std::string for error handling in release v1.0.6.
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.data());

    if (!err.empty())   { std::cerr << "Error: " << err << std::endl; } 
    if (!ret)           { exit(1); }

    parse_obj(attrib, shapes, materials);
}

Mesh::Mesh(std::string_view path, std::shared_ptr<Texture>& t)
    : Mesh(path)
{ 
    texture = t;
}

void Mesh::setTexture(std::shared_ptr<Texture>& t)
{
    texture = t;
}

const std::shared_ptr<Texture>& Mesh::getTexture() const
{
    return texture;
}

void Mesh::setFaces(std::vector<Face>& f)
{
    faces = f;
}

std::vector<Face>& Mesh::getFaces()
{
    return faces;
}