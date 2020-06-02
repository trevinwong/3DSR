#pragma once
#include "mesh.h"
#include "mat4.h"

// Represents an object in a world.
// Contains its own matrix O to multiply by the world frame to obtain the object frame.
class Object
{
    private:
        Mesh* mesh = nullptr;
        mat4 mat;
    public:
        Object() = default;
        Object(Mesh* m);
};
