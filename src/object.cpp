#include "object.h"

Object::Object(std::unique_ptr<Mesh> Mesh, std::unique_ptr<mat4> Mat)
    : mesh(std::move(Mesh)), mat(std::move(Mat))
{
}

Mesh* Object::getMesh() const
{
    return mesh.get();
}

mat4* Object::getMat() const
{
    return mat.get();
}