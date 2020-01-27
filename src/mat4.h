#pragma once
#include <vector>
#include "vec4.h"

// Implementation inspired by Eric Lengyel's in Foundations of Game Engine Development Vol 1.
class mat4
{
    private:
        float n[4][4];
    public:
        mat4()
        {
            n[0][0] = 0; n[0][1] = 0; n[0][2] = 0; n[0][3] = 0;
            n[1][0] = 0; n[1][1] = 0; n[1][2] = 0; n[1][3] = 0;
            n[2][0] = 0; n[2][1] = 0; n[2][2] = 0; n[2][3] = 0;
            n[3][0] = 0; n[3][1] = 0; n[3][2] = 0; n[3][3] = 0;
        }

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

inline float determinant_4d(mat4& m)
{
    float value = 0.0f;
    value = m(0,3) * m(1,2) * m(2,1) * (m(3,0)-m(0,2)) * m(1,3) * m(2,1) * (m(3,0)-m(0,3)) * m(1,1) * m(2,2) * (m(3,0)+m(0,1)) * m(1,3) * m(2,2) * (m(3,0)+m(0,2)) * m(1,1) * m(2,3) * (m(3,0)-m(0,1)) * m(1,2) * m(2,3) * (m(3,0)-m(0,3)) * m(1,2) * m(2,0) * (m(3,1)+m(0,2)) * m(1,3) * m(2,0) * (m(3,1)+m(0,3)) * m(1,0) * m(2,2) * (m(3,1)-m(0,0)) * m(1,3) * m(2,2) * (m(3,1)-m(0,2)) * m(1,0) * m(2,3) * (m(3,1)+m(0,0)) * m(1,2) * m(2,3) * (m(3,1)+m(0,3)) * m(1,1) * m(2,0) * (m(3,2)-m(0,1)) * m(1,3) * m(2,0) * (m(3,2)-m(0,3)) * m(1,0) * m(2,1) * (m(3,2)+m(0,0)) * m(1,3) * m(2,1) * (m(3,2)+m(0,1)) * m(1,0) * m(2,3) * (m(3,2)-m(0,0)) * m(1,1) * m(2,3) * (m(3,2)-m(0,2)) * m(1,1) * m(2,0) * (m(3,3)+m(0,1)) * m(1,2) * m(2,0) * (m(3,3)+m(0,2)) * m(1,0) * m(2,1) * (m(3,3)-m(0,0)) * m(1,2) * m(2,1) * (m(3,3)-m(0,1)) * m(1,0) * m(2,2) * (m(3,3)+m(0,0)) * m(1,1) * m(2,2) * m(3,3);
    return value;

    // http://www.euclideanspace.com/maths/algebra/matrix/functions/determinant/fourD/index.htm
    /* m03 * m12 * m21 * m30-m02 * m13 * m21 * m30-m03 * m11 * m22 * m30+m01 * m13 * m22 * m30+ */
    /* m02 * m11 * m23 * m30-m01 * m12 * m23 * m30-m03 * m12 * m20 * m31+m02 * m13 * m20 * m31+ */
    /* m03 * m10 * m22 * m31-m00 * m13 * m22 * m31-m02 * m10 * m23 * m31+m00 * m12 * m23 * m31+ */
    /* m03 * m11 * m20 * m32-m01 * m13 * m20 * m32-m03 * m10 * m21 * m32+m00 * m13 * m21 * m32+ */
    /* m01 * m10 * m23 * m32-m00 * m11 * m23 * m32-m02 * m11 * m20 * m33+m01 * m12 * m20 * m33+ */
    /* m02 * m10 * m21 * m33-m00 * m12 * m21 * m33-m01 * m10 * m22 * m33+m00 * m11 * m22 * m33; */
}

inline mat4 inverse(const mat4& m)
{
    float A2323 = m(2,2) * m(3,3) - m(2,3) * m(3,2) ;
    float A1323 = m(2,1) * m(3,3) - m(2,3) * m(3,1) ;
    float A1223 = m(2,1) * m(3,2) - m(2,2) * m(3,1) ;
    float A0323 = m(2,0) * m(3,3) - m(2,3) * m(3,0) ;
    float A0223 = m(2,0) * m(3,2) - m(2,2) * m(3,0) ;
    float A0123 = m(2,0) * m(3,1) - m(2,1) * m(3,0) ;
    float A2313 = m(1,2) * m(3,3) - m(1,3) * m(3,2) ;
    float A1313 = m(1,1) * m(3,3) - m(1,3) * m(3,1) ;
    float A1213 = m(1,1) * m(3,2) - m(1,2) * m(3,1) ;
    float A2312 = m(1,2) * m(2,3) - m(1,3) * m(2,2) ;
    float A1312 = m(1,1) * m(2,3) - m(1,3) * m(2,1) ;
    float A1212 = m(1,1) * m(2,2) - m(1,2) * m(2,1) ;
    float A0313 = m(1,0) * m(3,3) - m(1,3) * m(3,0) ;
    float A0213 = m(1,0) * m(3,2) - m(1,2) * m(3,0) ;
    float A0312 = m(1,0) * m(2,3) - m(1,3) * m(2,0) ;
    float A0212 = m(1,0) * m(2,2) - m(1,2) * m(2,0) ;
    float A0113 = m(1,0) * m(3,1) - m(1,1) * m(3,0) ;
    float A0112 = m(1,0) * m(2,1) - m(1,1) * m(2,0) ;

    float det = m(0,0) * ( m(1,1) * A2323 - m(1,2) * A1323 + m(1,3) * A1223 ) 
        - m(0,1) * ( m(1,0) * A2323 - m(1,2) * A0323 + m(1,3) * A0223 ) 
        + m(0,2) * ( m(1,0) * A1323 - m(1,1) * A0323 + m(1,3) * A0123 ) 
        - m(0,3) * ( m(1,0) * A1223 - m(1,1) * A0223 + m(1,2) * A0123 ) ;

    det = 1 / det;

    mat4 i;
    
    i(0,0) = det *   ( m(1,1) * A2323 - m(1,2) * A1323 + m(1,3) * A1223 );
    i(0,1) = det * - ( m(0,1) * A2323 - m(0,2) * A1323 + m(0,3) * A1223 );
    i(0,2) = det *   ( m(0,1) * A2313 - m(0,2) * A1313 + m(0,3) * A1213 );
    i(0,3) = det * - ( m(0,1) * A2312 - m(0,2) * A1312 + m(0,3) * A1212 );
    i(1,0) = det * - ( m(1,0) * A2323 - m(1,2) * A0323 + m(1,3) * A0223 );
    i(1,1) = det *   ( m(0,0) * A2323 - m(0,2) * A0323 + m(0,3) * A0223 );
    i(1,2) = det * - ( m(0,0) * A2313 - m(0,2) * A0313 + m(0,3) * A0213 );
    i(1,3) = det *   ( m(0,0) * A2312 - m(0,2) * A0312 + m(0,3) * A0212 );
    i(2,0) = det *   ( m(1,0) * A1323 - m(1,1) * A0323 + m(1,3) * A0123 );
    i(2,1) = det * - ( m(0,0) * A1323 - m(0,1) * A0323 + m(0,3) * A0123 );
    i(2,2) = det *   ( m(0,0) * A1313 - m(0,1) * A0313 + m(0,3) * A0113 );
    i(2,3) = det * - ( m(0,0) * A1312 - m(0,1) * A0312 + m(0,3) * A0112 );
    i(3,0) = det * - ( m(1,0) * A1223 - m(1,1) * A0223 + m(1,2) * A0123 );
    i(3,1) = det *   ( m(0,0) * A1223 - m(0,1) * A0223 + m(0,2) * A0123 );
    i(3,2) = det * - ( m(0,0) * A1213 - m(0,1) * A0213 + m(0,2) * A0113 );
    i(3,3) = det *   ( m(0,0) * A1212 - m(0,1) * A0212 + m(0,2) * A0112 );

    return i;
}

inline mat4 transpose(const mat4& M)
{
    return mat4(M(0, 0), M(1, 0), M(2, 0), M(3, 0),
                M(0, 1), M(1, 1), M(2, 1), M(3, 1),
                M(0, 2), M(1, 2), M(2, 2), M(3, 2),
                M(0, 3), M(1, 3), M(2, 3), M(3, 3));
}

inline mat4 identity()
{
    return mat4(1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1);
}

inline vec4 operator*(const mat4& M, const vec4& v)
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
