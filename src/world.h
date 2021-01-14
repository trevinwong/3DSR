#pragma once
#include "object.h"
#include "mesh.h"
#include "mat4.h"

class World
{
    public:
        World() = default;

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
