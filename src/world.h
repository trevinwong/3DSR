#pragma once
#include "object.h"
#include "mesh.h"
#include "mat4.h"

class World
{
    public:
        World() = default;

        // DEPRECATED
        void add_mesh_to_world(Mesh& mesh, const mat4& transformation);
        std::vector<std::pair<Mesh, mat4>>& get_meshes_in_world();

        void addObject(Object* object);
        std::vector<Object*>& getObjects();
        
        void set_light(const vec3& l);
        const vec3& get_light() const;

        void set_eye(const vec3& e);
        const vec3& get_eye() const;

        void set_look_at_pt(const vec3& lk_at);
        const vec3& get_look_at_pt() const;
    private:
        std::vector<Object*> objects;

        // DEPRECATED
        // A mesh and its transformation in the world.
        std::vector<std::pair<Mesh, mat4>> meshes_in_world;

        vec3 light;
        vec3 eye;
        vec3 look_at_pt;
        float t = 1.0f;
        float b = -t;
        float r = 1.0f;
        float l = -r;
        float n = 1.8f;
        float f = 10.0f;
};
