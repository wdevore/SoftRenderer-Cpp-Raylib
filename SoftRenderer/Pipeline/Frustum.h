#pragma once

#include "Matrix4f.h"
#include "Plane.h"

// This frustum is based on OpenGL's world-space.
// +Z is towards the Eye and is a right-handed system:
// the positive x-axis is to your right,
// the positive y-axis is up and
// the positive z-axis is backwards (i.e. behind the eye)

// OpenGL is Column-major, so we adopt that.
// (Column-major layout)
// Matrix derivation: https://www.songho.ca/opengl/gl_projectionmatrix.html
// https://www.mathematik.uni-marburg.de/~thormae/lectures/graphics1/graphics_6_1_eng_web.html#1
//
// From eye-space to NDC-space
//
// 0             1            2               3
// ----------------------------------------------------------------------
// 00{00} 2*n    04{01} 0     08{02} (r+l)    12(03) 0
//       -----                       -----
//       (r-l)                       (r-l)
//
// 01{10} 0      05{11} 2*n   09{12} (t+b)    13{13} 0
//                     -----         -----
//                     (t-b)         (t-b)
//
// 02{20} 0      06{21} 0     10{22} -(f+n)   14{23} -2(f*n)
//                                   ------          -------
//                                    (f-n)            (f-n)
//
// 03{30} 0      07{31} 0     11{32} -1       15{33} 0
//
// Note: cells 10 and 14 can be rewritten by multiplying by -1:
//
//                            10{22}  (f+n)   14{23}  2(f*n)
//                                   ------          -------
//                                    (n-f)            (n-f)
// Which is what is used in this class.

class Frustum
{
private:
    /* data */
public:
    float left{};
    float right{};
    float bottom{};
    float top{};
    float near{};
    float far{};
    float depth{};

    // These typically are the same as viewport dimensions
    float width{};
    float height{};

    Matrix4f volume{};
    Plane nearPlane{};

    Frustum(/* args */);
    ~Frustum();

    void SetWidth(float left, float right);
    void SetHeight(float top, float bottom);
    void SetNearPlane(float nx, float ny, float nz, float px, float py, float pz);

    void BuildProjectionMatrix();
    float GetViewDistance();
};
