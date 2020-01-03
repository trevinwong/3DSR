#pragma once
#include <vector>
#include "vec4.h"

// Implementation inspired by Eric Lengyel's in Foundations of Game Engine Development Vol 1.
class mat4
{
    private:
        float n[4][4];
    public:
        mat4() = default;

        // Stored in column-major order. In other words, specify it as you would a regular matrix, and this data structure will pack elements of
        // a column close together. First index is the column, second index is the row.
        mat4(float n00, float n01, float n02, float n03,
                float n10, float n11, float n12, float n13,
                float n20, float n21, float n22, float n23,
                float n30, float n31, float n32, float n33)
        {
            n[0][0] = n00; n[0][1] = n10; n[0][2] = n20; n[0][3] = n30;
            n[1][0] = n01; n[1][1] = n11; n[1][2] = n21; n[1][3] = n31;
            n[2][0] = n02; n[2][1] = n12; n[2][2] = n22; n[2][3] = n32;
            n[3][0] = n03; n[3][1] = n13; n[3][2] = n23; n[3][3] = n33;
        }

        // Construct a mat4 from 4 column vectors.
        mat4(const vec4& a, const vec4& b, const vec4& c, const vec4& d)
        {
            n[0][0] = a.x; n[0][1] = a.y; n[0][2] = a.z; n[0][3] = a.w;
            n[1][0] = b.x; n[1][1] = b.y; n[1][2] = b.z; n[1][3] = b.w;
            n[2][0] = c.x; n[2][1] = c.y; n[2][2] = c.z; n[2][3] = c.w;
            n[3][0] = d.x; n[3][1] = d.y; n[3][2] = d.z; n[3][3] = d.w;
        }

        // i = row, j = column
        float& operator()(int i, int j)
        {
            return (n[j][i]);
        }

        const float& operator()(int i, int j) const
        {
            return (n[j][i]);
        }

        // Return the column vector at the index j.
        vec4& operator[](int j)
        {
            return (*reinterpret_cast<vec4*>(n[j]));
        }

        const vec4& operator[](int j) const
        {
            return (*reinterpret_cast<const vec4*>(n[j]));
        }

        void print()
        {
            std::cout << "Mat4: " << std::endl;
            std::cout << n[0][0] << n[1][0] << n[2][0] << n[3][0] << std::endl;  
            std::cout << n[0][1] << n[1][1] << n[2][1] << n[3][1] << std::endl;  
            std::cout << n[0][2] << n[1][2] << n[2][2] << n[3][2] << std::endl;  
            std::cout << n[0][3] << n[1][3] << n[2][3] << n[3][3] << std::endl;  
        }
};

inline mat4 transpose(const mat4& M)
{
    return mat4(M(0, 0), M(1, 0), M(2, 0), M(3, 0),
                M(0, 1), M(1, 1), M(2, 1), M(3, 1),
                M(0, 2), M(1, 2), M(2, 2), M(3, 2),
                M(0, 3), M(1, 3), M(2, 3), M(3, 3));
}

inline vec4 operator*(const mat4& M, const vec4 v)
{
    return (vec4((M(0,0) * v.x) + (M(0,1) * v.y) + (M(0,2) * v.z) + (M(0,3) * v.w),
                 (M(1,0) * v.x) + (M(1,1) * v.y) + (M(1,2) * v.z) + (M(1,3) * v.w),
                 (M(2,0) * v.x) + (M(2,1) * v.y) + (M(2,2) * v.z) + (M(2,3) * v.w),
                 (M(3,0) * v.x) + (M(3,1) * v.y) + (M(3,2) * v.z) + (M(3,3) * v.w)));
}

inline mat4 operator*(const mat4& M0, const mat4& M1)
{
    std::vector<vec4> new_columns;
    for (int i = 0; i < 4; i++)
    {
        vec4 column = M1[i];
        new_columns.push_back(M0 * column);
    }
    return mat4(new_columns[0], new_columns[1], new_columns[2], new_columns[3]);
}
