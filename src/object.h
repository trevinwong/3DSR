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
        Object(std::shared_ptr<Mesh> Mesh, std::unique_ptr<mat4> Mat);

        std::shared_ptr<Mesh>& getMesh();
        std::unique_ptr<mat4>& getMat();
    private:
        std::shared_ptr<Mesh> mesh = nullptr;
        std::unique_ptr<mat4> mat = nullptr;
};
