#include "world.h"

void World::set_mesh(const Mesh& m)
{
    mesh = m;
}

const Mesh& World::get_mesh()
{
    return mesh;
}

void World::set_light(const vec3& l)
{
    light = l;
}

const vec3& World::get_light()
{
    return light;
}

void World::set_eye(const vec3& e)
{
    eye = e;
}

const vec3& World::get_eye()
{
    return eye;
}
