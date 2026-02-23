#pragma once

#include <string>
#include <vector>

#include "Vector3f.h"
#include "Point3f.h"
#include "AxisAngle4f.h"

class Matrix4f
{
public:
    // Column-Major (OpenGL) = mrc:
    // Logic: m[Column * 4 + Row]
    //        Col0  Col1  Col2  Col3
    // Row0   m00   m01   m02   m03
    // Row1   m10   m11   m12   m13
    // Row2   m20   m21   m22   m23
    // Row3   m30   m31   m32   m33

    // m[0] => m00 = Row 0, Col 0
    // m[1] => m10 = Row 1, Col 0
    // m[2] => m20 = Row 2, Col 0
    // m[3] => m30 = Row 3, Col 0
    //
    // m[4] => m01 = Row 0, Col 1
    // m[5] => m11 = Row 1, Col 1
    // m[6] => m21 = Row 2, Col 1
    // m[7] => m31 = Row 3, Col 1
    // etc...
    float m00, m01, m02, m03;
    float m10, m11, m12, m13;
    float m20, m21, m22, m23;
    float m30, m31, m32, m33;

    Matrix4f();
    Matrix4f(const Matrix4f &m1);
    Matrix4f(float m00, float m01, float m02, float m03,
             float m10, float m11, float m12, float m13,
             float m20, float m21, float m22, float m23,
             float m30, float m31, float m32, float m33);
    ~Matrix4f() = default;

    void setIdentity();
    void set(const Matrix4f &m1);
    void set(const AxisAngle4f &a1);
    void set(const Vector3f &trans);
    void setTranslation(const Vector3f &trans);

    void mul(const Matrix4f &m1);
    void mul(const Matrix4f &m1, const Matrix4f &m2);

    void invert();
    float determinant() const;

    void transform(Point3f &point) const;
    void transform(Vector3f &normal) const;

    void setRotation(AxisAngle4f a1);

    bool equals(const Matrix4f &m1) const;
    bool epsilonEquals(const Matrix4f &m1, float epsilon) const;

    std::string toString() const;
};