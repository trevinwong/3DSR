#pragma once
#include "mesh.h"
#include "mat4.h"

// Represents an object in a world.
// Contains its own matrix O to multiply by the world frame to obtain the object frame.
class Object
{
    public:
        Object() = default;
        Object(Mesh* m);

        Mesh* getMesh() const;
        mat4& getMat();
        const mat4& getMat() const;
    private:
        Mesh* mesh = nullptr;
        mat4 mat;
};
