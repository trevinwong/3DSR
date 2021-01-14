#pragma once
#include <vector>
#include <math.h>
#include "vec4.h"

#define PI 3.14159265

// Implementation inspired by Eric Lengyel's in Foundations of Game Engine Development Vol 1.
class mat4
{
    private:
        float n[4][4];
    public:
        mat4();

        // Column-major order, for easy retrieval of column vectors.
        mat4(float n00, float n01, float n02, float n03,
             float n10, float n11, float n12, float n13,
             float n20, float n21, float n22, float n23,
             float n30, float n31, float n32, float n33);
        mat4(const vec4& a, const vec4& b, const vec4& c, const vec4& d);

        float& operator()(int row, int col);
        const float& operator()(int row, int col) const;

        // Return the column vector at the index j.
        vec4& operator[](int j);
        const vec4& operator[](int j) const;
        
        void print();
};

mat4 inverse(const mat4& M);
mat4 transpose(const mat4& M);
mat4 identity();
mat4 makeXRotation(double deg);
mat4 makeYRotation(double deg);
mat4 makeZRotation(double deg);
mat4 makeScale(double sx, double sy, double sz);
mat4 makeTranslation(double tx, double ty, double tz);
mat4 transFactor(const mat4& M);
mat4 linFact(const mat4& M);
mat4 normalMatrix(const mat4& M);

vec4 operator*(const mat4& M, const vec4& v);
mat4 operator*(const mat4& M0, const mat4& M1);
