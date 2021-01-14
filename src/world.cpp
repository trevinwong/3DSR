#include "world.h"

void World::addObject(Object* object)
{
    objects.push_back(object);
}

std::vector<Object*>& World::getObjects()
{
    return objects;
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
