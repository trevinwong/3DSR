#include "object.h"

Object::Object(std::shared_ptr<Mesh> Mesh, std::unique_ptr<mat4> Mat)
    : mesh(std::move(Mesh)), mat(std::move(Mat))
{
}

std::shared_ptr<Mesh>& Object::getMesh()
{
    return mesh;
}

std::unique_ptr<mat4>& Object::getMat()
{
    return mat;
}