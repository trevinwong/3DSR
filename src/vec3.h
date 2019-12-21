#pragma once
#include <cmath>
#include <iostream>

// Implementation inspired by Eric Lengyel's in Foundations of Game Engine Development Vol 1.
class vec3
{
    public: 
        float x, y, z;

        vec3() = default;

        vec3(float a, float b, float c)
            : x(a), y(b), z(c)
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

        vec3& operator/=(float s)
        {
            s = 1.0f / s;
            x *= s;
            y *= s;
            z *= s;
            return *this;
        }

        vec3 operator*(float s)
        {
            return vec3(x * s, y * s, z * s);
        }

        vec3& operator+=(vec3& v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        vec3 operator+(vec3& v)
        {
            return vec3(x + v.x, y + v.y, z + v.z);
        }

        vec3& operator-=(vec3& v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }

        vec3 operator-(vec3& v)
        {
            return vec3(x - v.x, y - v.y, z - v.z);
        }

        float length()
        {
            return std::sqrt((x*x) + (y*y) + (z*z));
        }

        void normalize_inplace()
        {
            *this /= length();
        }

        void print() const
        {
            std::cout << "Vec3" << ": x = " << x << ", y = " << y << ", z = " << z << std::endl;
        }
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
