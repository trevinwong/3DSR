#pragma once

static mat4 lookAt(vec3 eye, vec3 target, vec3 up = vec3(0,1,0))
{
    // Translate eye position back to origin.
    // Our calculations assume that our eye is positioned at the origin and looks down the -Z axis.
    mat4 mT(    1, 0, 0, -eye.x,
                0, 1, 0, -eye.y,
                0, 0, 1, -eye.z,
                0, 0, 0, 1
            );

    vec3 forward = (eye - target).normalize();

    // Be very careful with the order of these cross products.
    vec3 left = cross(up.normalize(), forward).normalize();
    up = cross(forward, left);

    mat4 mR(   
                left.x, up.x, forward.x, 0,
                left.y, up.y, forward.y, 0,
                left.z, up.z, forward.z, 0,
                0, 0, 0, 1
            );

    return inverse(mR) * mT;
}

// The math is from: http://www.songho.ca/opengl/gl_projectionmatrix.html#perspective
// NOTE: this assumes n and f are both positive!!!
// NOTE: this transforms the coordinates into NDCS coordinates, which FLIPS the direction of our wn-axis, essentially
// Our camera typically looks down the negative wn-axis but after transforming the coordinates it looks down the positive wn-axis.
// This means your wn-buffer implementation needs to change: closer coordinates should be SMALLER, not LARGER
// Construct the perspective matrix.
mat4 perspective(float t = 1.0f, float r = 1.0f, float n = 1.8f, float f = 10.0f)
{
    float b = -t;
    float l = -r;

    mat4 mat(   
                        2*n/(r-l), 0,         (r+l)/(r-l),      0,
                        0,         2*n/(t-b), (t+b)/(t-b),      0,
                        0,         0,         -(f + n)/(f - n), -2*(f * n)/(f - n),
                        0,         0,         -1,               0
            );
    return mat;
}

// Transforms the canonical cube (which ranges from [-1,-1,-1] to [1,1,1]) to range from [0,0,0] to [W,H,1].
mat4 viewport(Frame& frame)
{
    mat4 mat(   
                    frame.w/2, 0,         0,   (frame.w)/2,
                    0,         frame.h/2, 0,   (frame.h)/2,
                    0,         0,         1/2, 1/2,
                    0,         0,         0,   1
            );
    return mat;
}