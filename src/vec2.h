#pragma once
#include "vec4.h"
#include "vec3.h"
#include <iostream>

// Implementation inspired by Eric Lengyel's in Foundations of Game Engine Development Vol 1.
class vec2
{
    public: 
        vec2() = default;

        vec2(float X, float Y)
            : x(X), y(Y)
        {
        }

        vec2(const vec3& v)
            : x(v.x), y(v.y)
        {
        }

        vec2(const vec4& v)
            : x(v.x), y(v.y)
        {
        }

        float& operator[](int i)
        {
            return ((&x)[i]);
        }

        const float& operator[](int i) const
        {
            return ((&x)[i]);
        }

        vec2& operator*=(float s)
        {
            x *= s;
            y *= s;
            return *this;
        }

        vec2 operator*(float s) const
        {
            return vec2(x * s, y * s);
        }

        vec2& operator/=(float s)
        {
            s = 1.0f / s;
            x *= s;
            y *= s;
            return *this;
        }

        vec2 operator/(float s) const
        {
            s = 1.0f / s;
            return vec2(x * s, y * s);
        }

        vec2& operator+=(const vec2& v)
        {
            x += v.x;
            y += v.y;
            return *this;
        }

        vec2 operator+(const vec2& v) const
        {
            return vec2(x + v.x, y + v.y);
        }

        vec2& operator-=(const vec2& v)
        {
            x -= v.x;
            y -= v.y;
            return *this;
        }

        vec2 operator-(const vec2& v) const
        {
            return vec2(x - v.x, y - v.y);
        }

        void print() const
        {
            std::cout << "Vec2" << ": x = " << x << ", y = " << y << std::endl;
        }

        float x = 0;
        float y = 0;
};

inline float dot(const vec2& a, const vec2& b)
{
    return ((a.x * b.x) + (a.y * b.y));
}

// We are given two vectors with z = 0, and want to calculate their cross product.
// This is equivalent to the 2x2 determinant formed by the vectors created by their x and y components.
// This is also equivalent to the area of the parallelogram formed by the two vectors.
// This is also equivalent to comparing b to the edge function of a.
inline float cross(const vec2& a, const vec2& b)
{
    return (a.x * b.y) - (a.y * b.x);
}
