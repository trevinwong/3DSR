#pragma once
#include "shader.h"
#include "graphics.h"
#include "vec2.h"
#include <vector>

class PhongShader : public Shader
{
public:
	PhongShader(World& World, Frame& Frame) :
		Shader(World, Frame), normals(3, vec3()), world_positions(3, vec3()), uvs(3, vec3())
	{
	}

	virtual ~PhongShader() override
	{
	}

	vec4 vertex(const Vertex& vertex, const mat4& model, int num_vert) override
	{
		// TODO: pass in t,b,l,r,n,f for perspective
		// TODO: do actual clipping?
		vec4 model_coords = model * vertex.position;
		vec4 model_normals = inverse(transpose(model)) * vertex.normal;
		vec4 clip_coords = perspective() * lookAt(world.get_eye(), world.get_look_at_pt()) * model_coords;
		vec4 ndcs = clip_coords / clip_coords.w;
		vec4 viewport_coords = viewport(frame) * ndcs;
		viewport_coords.x = (int)viewport_coords.x; // Convert to int to avoid black gaps between triangles.
		viewport_coords.y = (int)viewport_coords.y; // Convert to int to avoid black gaps between triangles.
		viewport_coords.w = clip_coords.w; // Keep wn (aka -z) for perspective-correct linear interpolation.

		// https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/perspective-correct-interpolation-vertex-attributes
		// we must divide vertex attributes by z first before linearly interpolating
		normals[num_vert] = model_normals / viewport_coords.w;
		world_positions[num_vert] = vertex.position / viewport_coords.w;
		uvs[num_vert] = vertex.uv / viewport_coords.w;

		return viewport_coords;
	}

	bool fragment(const vec4& barycentric, uint32_t& color) override
	{
		vec3 normal = (((normals[0] * barycentric.x) + (normals[1] * barycentric.y) + (normals[2] * barycentric.z)) * barycentric.w).normalize();
		vec3 world_position = ((world_positions[0] * barycentric.x) + (world_positions[1] * barycentric.y) + (world_positions[2] * barycentric.z)) * barycentric.w;
		vec2 uv = ((uvs[0] * barycentric.x) + (uvs[1] * barycentric.y) + (uvs[2] * barycentric.z)) * barycentric.w;

		vec3 to_eye = (world.get_eye() - world_position).normalize();
		vec3 to_light = (world.get_light() - world_position).normalize();

		vec3 proj_of_to_light_on_normal = (normal * dot(to_light, normal));
		vec3 to_reflection_pos = (proj_of_to_light_on_normal - to_light) * 2;
		vec3 reflected = to_light + to_reflection_pos;

		float diffuse = std::max(dot(normal, to_light), 0.0f);
		float specular = std::pow(std::max(dot(to_eye, reflected), 0.0f), 2.0f);

		float ka = 0.1f;
		float kd = 0.5f;
		float ks = 0.4f;

		float phong_term = ka + (kd * diffuse) + (ks * specular);

		float r = 255.0f, g = 255.0f, b = 255.0f;
        if (texture != nullptr)
        {
            uint32_t u = (uint32_t) std::floor(uv.x * texture->width);
            uint32_t v = (uint32_t) std::floor(uv.y * texture->height);

            // NotCamelCase/SoftLit/Texture.cpp
            int idx = ((v * texture->width) + u) * texture->channels;
            r = (float) texture->data[idx++];
            g = (float) texture->data[idx++];
            b = (float) texture->data[idx++];
        }

		color = SDL_MapRGBA(frame.pixel_format, r * phong_term, g * phong_term, b * phong_term, 255);
		return false;
	}
private:
	std::vector<vec3> normals;
	std::vector<vec3> world_positions;
	std::vector<vec2> uvs;
};