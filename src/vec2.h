#pragma once
#include "vec3.h"

// Implementation inspired by Eric Lengyel's in Foundations of Game Engine Development Vol 1.
class vec2
{
    public: 
        float x = 0, y = 0;

        vec2() = default;

        vec2(float a, float b)
            : x(a), y(b)
        {
        }

        vec2(const vec3& v)
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

        vec2& operator/=(float s)
        {
            s = 1.0f / s;
            x *= s;
            y *= s;
            return *this;
        }

        vec2 operator*(float s)
        {
            return vec2(x * s, y * s);
        }

        vec2& operator+=(const vec2& v)
        {
            x += v.x;
            y += v.y;
            return *this;
        }

        vec2 operator+(const vec2& v)
        {
            return vec2(x + v.x, y + v.y);
        }

        vec2& operator-=(const vec2& v)
        {
            x -= v.x;
            y -= v.y;
            return *this;
        }

        vec2 operator-(const vec2& v)
        {
            return vec2(x - v.x, y - v.y);
        }

        void print() const
        {
            std::cout << "Vec2" << ": x = " << x << ", y = " << y << std::endl;
        }
};

inline float dot(const vec2& a, const vec2& b)
{
    return ((a.x * b.x) + (a.y * b.y));
}
