#pragma once

// Implementation inspired by Eric Lengyel's in Foundations of Game Engine Development Vol 1.
class vec4
{
    public: 
        float x, y, z, w;

        vec4() = default;

        vec4(float a, float b, float c, float d)
            : x(a), y(b), z(c), w(d)
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

        vec4& operator/=(float s)
        {
            s = 1.0f / s;
            x *= s;
            y *= s;
            z *= s;
            w *= s;
            return *this;
        }

        vec4 operator*(float s)
        {
            return vec4(x * s, y * s, z * s, w * s);
        }

        vec4& operator+=(vec4& v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            w += v.w;
            return *this;
        }

        vec4 operator+(vec4& v)
        {
            return vec4(x + v.x, y + v.y, z + v.z, w + v.w);
        }

        vec4& operator-=(vec4& v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            w -= v.w;
            return *this;
        }

        vec4 operator-(vec4& v)
        {
            return vec4(x - v.x, y - v.y, z - v.z, w - v.w);
        }
};

inline float dot(const vec4& a, const vec4& b)
{
    return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w));
}

// Take the first vector's w as the w of the new vector.
inline vec4 cross(const vec4& a, const vec4& b)
{
    return vec4((a.y * b.z - a.z * b.y),
                (a.z * b.x - a.x * b.z), 
                (a.x * b.y - a.y * b.x),
                a.w);
}