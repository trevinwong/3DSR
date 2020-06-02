#pragma once
#include <iostream>

// Implementation inspired by Eric Lengyel's in Foundations of Game Engine Development Vol 1.
class vec4
{
    public: 
        vec4() = default;

        vec4(float X, float Y, float Z, float W)
            : x(X), y(Y), z(Z), w(W)
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

        vec4& operator*=(float s)
        {
            x *= s;
            y *= s;
            z *= s;
            w *= s;
            return *this;
        }

        vec4 operator*(float s) const
        {
            return vec4(x * s, y * s, z * s, w * s);
        }

        vec4& operator/=(float s)
        {
            s = 1.0f / s;
            x *= s;
            y *= s;
            z *= s;
            w *= s;
            return *this;
        }

        vec4 operator/(float s) const
        {
            s = 1.0f / s;
            return vec4(x * s, y * s, z * s, w * s);
        }

        vec4& operator+=(const vec4& v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            w += v.w;
            return *this;
        }

        vec4 operator+(const vec4& v) const
        {
            return vec4(x + v.x, y + v.y, z + v.z, w + v.w);
        }

        vec4& operator-=(const vec4& v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            w -= v.w;
            return *this;
        }

        vec4 operator-(const vec4& v) const
        {
            return vec4(x - v.x, y - v.y, z - v.z, w - v.w);
        }

        void print() const
        {
            std::cout << "Vec4" << ": x = " << x << ", y = " << y << ", z = " << z << ", w = " << w << std::endl;
        }

        float x = 0;
        float y = 0;
        float z = 0;
        float w = 0;
};

inline vec4 cross(const vec4& a, const vec4& b)
{
    return vec4((a.y * b.z - a.z * b.y),
                (a.z * b.x - a.x * b.z), 
                (a.x * b.y - a.y * b.x),
                0);
}