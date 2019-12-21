#pragma once

// Implementation inspired by Eric Lengyel's in Foundations of Game Engine Development Vol 1.
class vec2
{
    public: 
        float x, y;

        vec2() = default;

        vec2(float a, float b)
            : x(a), y(b)
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

        vec2& operator+=(vec2& v)
        {
            x += v.x;
            y += v.y;
            return *this;
        }

        vec2 operator+(vec2& v)
        {
            return vec2(x + v.x, y + v.y);
        }

        vec2& operator-=(vec2& v)
        {
            x -= v.x;
            y -= v.y;
            return *this;
        }

        vec2 operator-(vec2& v)
        {
            return vec2(x - v.x, y - v.y);
        }
};

inline float dot(const vec2& a, const vec2& b)
{
    return ((a.x * b.x) + (a.y * b.y));
}
