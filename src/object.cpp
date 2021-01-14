#include "object.h"

Object::Object(Mesh* m)
    : mesh(m)
{
}

Mesh* Object::getMesh() const
{
    return mesh;
}

mat4& Object::getMat() 
{
    return mat;
}

const mat4& Object::getMat() const
{
    return mat;
}