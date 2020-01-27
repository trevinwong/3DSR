#include "world.h"

void World::add_mesh_to_world(Mesh& mesh, const mat4& transformation)
{
    meshes_in_world.push_back(std::pair(mesh, transformation));
}

std::vector<std::pair<Mesh, mat4>>& World::get_meshes_in_world()
{
    return meshes_in_world;
}

void World::set_light(const vec3& l)
{
    light = l;
}

const vec3& World::get_light() const
{
    return light;
}

void World::set_eye(const vec3& e)
{
    eye = e;
}

const vec3& World::get_eye() const
{
    return eye;
}

void World::set_look_at_pt(const vec3& lk_at)
{
    look_at_pt = lk_at;
}

const vec3& World::get_look_at_pt() const
{
    return look_at_pt;
}
