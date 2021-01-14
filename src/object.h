#pragma once
#include <memory>
#include "mesh.h"
#include "mat4.h"

// Represents an object in a world.
// Contains its own matrix O to multiply by the world frame to obtain the object frame.
class Object
{
    public:
        Object() = default;
        Object(std::unique_ptr<Mesh> Mesh, std::unique_ptr<mat4> Mat);

        Mesh* getMesh() const;
        mat4* getMat() const;
    private:
        std::unique_ptr<Mesh> mesh;
        std::unique_ptr<mat4> mat;
};
