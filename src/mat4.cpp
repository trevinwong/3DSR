#include "mat4.h"

mat4::mat4()
{
            n[0][0] = 1; n[0][1] = 0; n[0][2] = 0; n[0][3] = 0;
            n[1][0] = 0; n[1][1] = 1; n[1][2] = 0; n[1][3] = 0;
            n[2][0] = 0; n[2][1] = 0; n[2][2] = 1; n[2][3] = 0;
            n[3][0] = 0; n[3][1] = 0; n[3][2] = 0; n[3][3] = 1;
}


mat4::mat4(float n00, float n01, float n02, float n03,
           float n10, float n11, float n12, float n13,
           float n20, float n21, float n22, float n23,
           float n30, float n31, float n32, float n33)
{
    n[0][0] = n00; n[0][1] = n10; n[0][2] = n20; n[0][3] = n30;
    n[1][0] = n01; n[1][1] = n11; n[1][2] = n21; n[1][3] = n31;
    n[2][0] = n02; n[2][1] = n12; n[2][2] = n22; n[2][3] = n32;
    n[3][0] = n03; n[3][1] = n13; n[3][2] = n23; n[3][3] = n33;
}

mat4::mat4(const vec4& a, const vec4& b, const vec4& c, const vec4& d)
{
    n[0][0] = a.x; n[0][1] = a.y; n[0][2] = a.z; n[0][3] = a.w;
    n[1][0] = b.x; n[1][1] = b.y; n[1][2] = b.z; n[1][3] = b.w;
    n[2][0] = c.x; n[2][1] = c.y; n[2][2] = c.z; n[2][3] = c.w;
    n[3][0] = d.x; n[3][1] = d.y; n[3][2] = d.z; n[3][3] = d.w;
}

float& mat4::operator()(int i, int j)
{
    return (n[j][i]);
}

const float& mat4::operator()(int i, int j) const
{
    return (n[j][i]);
}

vec4& mat4::operator[](int j)
{
    return (*reinterpret_cast<vec4*>(n[j]));
}

const vec4& mat4::operator[](int j) const
{
    return (*reinterpret_cast<const vec4*>(n[j]));
}

void mat4::print()
{
    std::cout << "Mat4: " << std::endl;
    std::cout << n[0][0] << n[1][0] << n[2][0] << n[3][0] << std::endl;  
    std::cout << n[0][1] << n[1][1] << n[2][1] << n[3][1] << std::endl;  
    std::cout << n[0][2] << n[1][2] << n[2][2] << n[3][2] << std::endl;  
    std::cout << n[0][3] << n[1][3] << n[2][3] << n[3][3] << std::endl;  
}

mat4 inverse(const mat4& M)
{
    float A2323 = M(2,2) * M(3,3) - M(2,3) * M(3,2);
    float A1323 = M(2,1) * M(3,3) - M(2,3) * M(3,1);
    float A1223 = M(2,1) * M(3,2) - M(2,2) * M(3,1);
    float A0323 = M(2,0) * M(3,3) - M(2,3) * M(3,0);
    float A0223 = M(2,0) * M(3,2) - M(2,2) * M(3,0);
    float A0123 = M(2,0) * M(3,1) - M(2,1) * M(3,0);
    float A2313 = M(1,2) * M(3,3) - M(1,3) * M(3,2);
    float A1313 = M(1,1) * M(3,3) - M(1,3) * M(3,1);
    float A1213 = M(1,1) * M(3,2) - M(1,2) * M(3,1);
    float A2312 = M(1,2) * M(2,3) - M(1,3) * M(2,2);
    float A1312 = M(1,1) * M(2,3) - M(1,3) * M(2,1);
    float A1212 = M(1,1) * M(2,2) - M(1,2) * M(2,1);
    float A0313 = M(1,0) * M(3,3) - M(1,3) * M(3,0);
    float A0213 = M(1,0) * M(3,2) - M(1,2) * M(3,0);
    float A0312 = M(1,0) * M(2,3) - M(1,3) * M(2,0);
    float A0212 = M(1,0) * M(2,2) - M(1,2) * M(2,0);
    float A0113 = M(1,0) * M(3,1) - M(1,1) * M(3,0);
    float A0112 = M(1,0) * M(2,1) - M(1,1) * M(2,0);

    float det = M(0,0) * ( M(1,1) * A2323 - M(1,2) * A1323 + M(1,3) * A1223 ) 
              - M(0,1) * ( M(1,0) * A2323 - M(1,2) * A0323 + M(1,3) * A0223 ) 
              + M(0,2) * ( M(1,0) * A1323 - M(1,1) * A0323 + M(1,3) * A0123 ) 
              - M(0,3) * ( M(1,0) * A1223 - M(1,1) * A0223 + M(1,2) * A0123 );
    det = 1 / det;

    mat4 i;    
    i(0,0) = det *   ( M(1,1) * A2323 - M(1,2) * A1323 + M(1,3) * A1223 );
    i(0,1) = det * - ( M(0,1) * A2323 - M(0,2) * A1323 + M(0,3) * A1223 );
    i(0,2) = det *   ( M(0,1) * A2313 - M(0,2) * A1313 + M(0,3) * A1213 );
    i(0,3) = det * - ( M(0,1) * A2312 - M(0,2) * A1312 + M(0,3) * A1212 );
    i(1,0) = det * - ( M(1,0) * A2323 - M(1,2) * A0323 + M(1,3) * A0223 );
    i(1,1) = det *   ( M(0,0) * A2323 - M(0,2) * A0323 + M(0,3) * A0223 );
    i(1,2) = det * - ( M(0,0) * A2313 - M(0,2) * A0313 + M(0,3) * A0213 );
    i(1,3) = det *   ( M(0,0) * A2312 - M(0,2) * A0312 + M(0,3) * A0212 );
    i(2,0) = det *   ( M(1,0) * A1323 - M(1,1) * A0323 + M(1,3) * A0123 );
    i(2,1) = det * - ( M(0,0) * A1323 - M(0,1) * A0323 + M(0,3) * A0123 );
    i(2,2) = det *   ( M(0,0) * A1313 - M(0,1) * A0313 + M(0,3) * A0113 );
    i(2,3) = det * - ( M(0,0) * A1312 - M(0,1) * A0312 + M(0,3) * A0112 );
    i(3,0) = det * - ( M(1,0) * A1223 - M(1,1) * A0223 + M(1,2) * A0123 );
    i(3,1) = det *   ( M(0,0) * A1223 - M(0,1) * A0223 + M(0,2) * A0123 );
    i(3,2) = det * - ( M(0,0) * A1213 - M(0,1) * A0213 + M(0,2) * A0113 );
    i(3,3) = det *   ( M(0,0) * A1212 - M(0,1) * A0212 + M(0,2) * A0112 );

    return i;
}

mat4 transpose(const mat4& M)
{
    return mat4(M(0, 0), M(1, 0), M(2, 0), M(3, 0),
                M(0, 1), M(1, 1), M(2, 1), M(3, 1),
                M(0, 2), M(1, 2), M(2, 2), M(3, 2),
                M(0, 3), M(1, 3), M(2, 3), M(3, 3));
}

mat4 identity()
{
    return mat4(1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1);
}

mat4 makeXRotation(double deg)
{
    double rad = deg * PI / 180.0;
    return mat4(1, 0,        0,         0,
                0, cos(rad), -sin(rad), 0,
                0, sin(rad), cos(rad),  0,
                0, 0,        0,         1);
}
mat4 makeYRotation(double deg)
{
    double rad = deg * PI / 180.0;
    return mat4(cos(rad),  0, sin(rad), 0,
                0,         1, 0,        0,
                -sin(rad), 0, cos(rad), 0,
                0,         0, 0,        1);
}
mat4 makeZRotation(double deg)
{
    double rad = deg * PI / 180.0;
    return mat4(cos(rad),  -sin(rad), 0, 0,
                sin(rad),  cos(rad),  0, 0,
                0,         0,         1, 0,
                0,         0,         0, 1);
}

mat4 makeScale(double sx, double sy, double sz)
{
    return mat4(sx, 0,  0,  0,
                0,  sy, 0,  0,
                0,  0,  sz, 0,
                0,  0,  0,  1);
}
mat4 makeTranslation(double tx, double ty, double tz)
{
    return mat4(1, 0, 0, tx,
                0, 1, 0, ty,
                0, 0, 1, tz,
                0, 0, 0, 1);
}

mat4 transFactor(const mat4& M)
{
    return mat4(1, 0, 0, M(0,3),
                0, 1, 0, M(1,3),
                0, 0, 1, M(2,3),
                0, 0, 0, 1);
}

mat4 linFact(const mat4& M)
{
    return mat4(M(0,0), M(0,1), M(0,2), 0,
                M(1,0), M(1,1), M(1,2), 0,
                M(2,0), M(2,1), M(2,2), 0,
                0,      0,      0,      1);
}

mat4 normalMatrix(const mat4& M)
{
    return inverse(transpose(linFact(M)));
}

vec4 operator*(const mat4& M, const vec4& v)
{
    return (vec4((M(0,0) * v.x) + (M(0,1) * v.y) + (M(0,2) * v.z) + (M(0,3) * v.w),
                 (M(1,0) * v.x) + (M(1,1) * v.y) + (M(1,2) * v.z) + (M(1,3) * v.w),
                 (M(2,0) * v.x) + (M(2,1) * v.y) + (M(2,2) * v.z) + (M(2,3) * v.w),
                 (M(3,0) * v.x) + (M(3,1) * v.y) + (M(3,2) * v.z) + (M(3,3) * v.w)));
}

mat4 operator*(const mat4& M0, const mat4& M1)
{
    std::vector<vec4> new_columns;
    for (int i = 0; i < 4; i++)
    {
        vec4 column = M1[i];
        new_columns.push_back(M0 * column);
    }
    return mat4(new_columns[0], new_columns[1], new_columns[2], new_columns[3]);
}
