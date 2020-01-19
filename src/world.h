#pragma once
#include "mesh.h"

class World
{
    public:
        World() = default;
        void set_mesh(const Mesh& m);
        const Mesh& get_mesh();

        void set_light(const vec3& l);
        const vec3& get_light();

        void set_eye(const vec3& e);
        const vec3& get_eye();
    private:
        // Could be expanded into a list of meshes, but we only render 1 mesh at a time in this world
        Mesh mesh;
        vec3 light;
        vec3 eye;
        float t = 1.0f;
        float b = -t;
        float r = 1.0f;
        float l = -r;
        float n = 1.8f;
        float f = 10.0f;
};
