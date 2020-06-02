#pragma once
#include "vec4.h"
#include <cmath>
#include <iostream>

// TODO: swizzling? converting vec4 to vec3 and a vec3 to vec4 without getting circular dependencies
// Implementation inspired by Eric Lengyel's in Foundations of Game Engine Development Vol 1.
class vec3
{
    public: 
        vec3() = default;

        vec3(float X, float Y, float Z)
            : x(X), y(Y), z(Z)
        {
        }

        vec3(const vec4& v)
            : x(v.x), y(v.y), z(v.z)
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

        vec3& operator*=(float s)
        {
            x *= s;
            y *= s;
            z *= s;
            return *this;
        }

        vec3 operator*(float s) const
        {
            return vec3(x * s, y * s, z * s);
        }

        vec3& operator/=(float s)
        {
            s = 1.0f / s;
            x *= s;
            y *= s;
            z *= s;
            return *this;
        }

        vec3 operator/(float s) const
        {
            s = 1.0f / s;
            return vec3(x * s, y * s, z * s);
        }

        vec3& operator+=(const vec3& v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        vec3 operator+(const vec3& v) const
        {
            return vec3(x + v.x, y + v.y, z + v.z);
        }

        vec3& operator-=(const vec3& v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }

        vec3 operator-(const vec3& v) const
        {
            return vec3(x - v.x, y - v.y, z - v.z);
        }

        float length() const
        {
            return std::sqrt((x*x) + (y*y) + (z*z));
        }

        vec3 normalize() const
        {
            return *this / length();
        }

        void normalize_inplace()
        {
            *this /= length();
        }

        void print() const
        {
            std::cout << "Vec3" << ": x = " << x << ", y = " << y << ", z = " << z << std::endl;
        }

        float x = 0;
        float y = 0;
        float z = 0;
};

inline float dot(const vec3& a, const vec3& b)
{
    return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}

inline vec3 cross(const vec3& a, const vec3& b)
{
    return vec3((a.y * b.z - a.z * b.y),
                (a.z * b.x - a.x * b.z), 
                (a.x * b.y - a.y * b.x));
}
