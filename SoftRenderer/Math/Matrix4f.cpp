#include "Matrix4f.h"
#include <cmath>
#include <sstream>

Matrix4f::Matrix4f()
{
    setIdentity();
}

Matrix4f::Matrix4f(const Matrix4f &m1)
{
    set(m1);
}

Matrix4f::Matrix4f(float m00, float m01, float m02, float m03,
                   float m10, float m11, float m12, float m13,
                   float m20, float m21, float m22, float m23,
                   float m30, float m31, float m32, float m33)
{
    this->m00 = m00;
    this->m01 = m01;
    this->m02 = m02;
    this->m03 = m03;
    this->m10 = m10;
    this->m11 = m11;
    this->m12 = m12;
    this->m13 = m13;
    this->m20 = m20;
    this->m21 = m21;
    this->m22 = m22;
    this->m23 = m23;
    this->m30 = m30;
    this->m31 = m31;
    this->m32 = m32;
    this->m33 = m33;
}

void Matrix4f::setIdentity()
{
    m00 = 1.0f;
    m01 = 0.0f;
    m02 = 0.0f;
    m03 = 0.0f;
    m10 = 0.0f;
    m11 = 1.0f;
    m12 = 0.0f;
    m13 = 0.0f;
    m20 = 0.0f;
    m21 = 0.0f;
    m22 = 1.0f;
    m23 = 0.0f;
    m30 = 0.0f;
    m31 = 0.0f;
    m32 = 0.0f;
    m33 = 1.0f;
}

void Matrix4f::set(const Matrix4f &m1)
{
    m00 = m1.m00;
    m01 = m1.m01;
    m02 = m1.m02;
    m03 = m1.m03;
    m10 = m1.m10;
    m11 = m1.m11;
    m12 = m1.m12;
    m13 = m1.m13;
    m20 = m1.m20;
    m21 = m1.m21;
    m22 = m1.m22;
    m23 = m1.m23;
    m30 = m1.m30;
    m31 = m1.m31;
    m32 = m1.m32;
    m33 = m1.m33;
}

void Matrix4f::set(const AxisAngle4f &a1)
{
    float c = std::cos(a1.angle);
    float s = std::sin(a1.angle);
    float t = 1.0f - c;
    float x = a1.x;
    float y = a1.y;
    float z = a1.z;

    // Normalize axis if needed, but assuming input is valid or user responsibility
    // Standard Rodrigues rotation formula
    m00 = c + x * x * t;
    m11 = c + y * y * t;
    m22 = c + z * z * t;

    float tmp1 = x * y * t;
    float tmp2 = z * s;
    m10 = tmp1 + tmp2;
    m01 = tmp1 - tmp2;

    tmp1 = x * z * t;
    tmp2 = y * s;
    m20 = tmp1 - tmp2;
    m02 = tmp1 + tmp2;

    tmp1 = y * z * t;
    tmp2 = x * s;
    m21 = tmp1 + tmp2;
    m12 = tmp1 - tmp2;

    m03 = 0.0f;
    m13 = 0.0f;
    m23 = 0.0f;
    m30 = 0.0f;
    m31 = 0.0f;
    m32 = 0.0f;
    m33 = 1.0f;
}

void Matrix4f::set(const Vector3f &trans)
{
    setIdentity();
    m03 = trans.x;
    m13 = trans.y;
    m23 = trans.z;
}

void Matrix4f::setTranslation(const Vector3f &trans)
{
    m03 = trans.x;
    m13 = trans.y;
    m23 = trans.z;
}

void Matrix4f::mul(const Matrix4f &m1)
{
    mul(*this, m1);
}

void Matrix4f::mul(const Matrix4f &m1, const Matrix4f &m2)
{
    float nm00 = m1.m00 * m2.m00 + m1.m01 * m2.m10 + m1.m02 * m2.m20 + m1.m03 * m2.m30;
    float nm01 = m1.m00 * m2.m01 + m1.m01 * m2.m11 + m1.m02 * m2.m21 + m1.m03 * m2.m31;
    float nm02 = m1.m00 * m2.m02 + m1.m01 * m2.m12 + m1.m02 * m2.m22 + m1.m03 * m2.m32;
    float nm03 = m1.m00 * m2.m03 + m1.m01 * m2.m13 + m1.m02 * m2.m23 + m1.m03 * m2.m33;

    float nm10 = m1.m10 * m2.m00 + m1.m11 * m2.m10 + m1.m12 * m2.m20 + m1.m13 * m2.m30;
    float nm11 = m1.m10 * m2.m01 + m1.m11 * m2.m11 + m1.m12 * m2.m21 + m1.m13 * m2.m31;
    float nm12 = m1.m10 * m2.m02 + m1.m11 * m2.m12 + m1.m12 * m2.m22 + m1.m13 * m2.m32;
    float nm13 = m1.m10 * m2.m03 + m1.m11 * m2.m13 + m1.m12 * m2.m23 + m1.m13 * m2.m33;

    float nm20 = m1.m20 * m2.m00 + m1.m21 * m2.m10 + m1.m22 * m2.m20 + m1.m23 * m2.m30;
    float nm21 = m1.m20 * m2.m01 + m1.m21 * m2.m11 + m1.m22 * m2.m21 + m1.m23 * m2.m31;
    float nm22 = m1.m20 * m2.m02 + m1.m21 * m2.m12 + m1.m22 * m2.m22 + m1.m23 * m2.m32;
    float nm23 = m1.m20 * m2.m03 + m1.m21 * m2.m13 + m1.m22 * m2.m23 + m1.m23 * m2.m33;

    float nm30 = m1.m30 * m2.m00 + m1.m31 * m2.m10 + m1.m32 * m2.m20 + m1.m33 * m2.m30;
    float nm31 = m1.m30 * m2.m01 + m1.m31 * m2.m11 + m1.m32 * m2.m21 + m1.m33 * m2.m31;
    float nm32 = m1.m30 * m2.m02 + m1.m31 * m2.m12 + m1.m32 * m2.m22 + m1.m33 * m2.m32;
    float nm33 = m1.m30 * m2.m03 + m1.m31 * m2.m13 + m1.m32 * m2.m23 + m1.m33 * m2.m33;

    m00 = nm00;
    m01 = nm01;
    m02 = nm02;
    m03 = nm03;
    m10 = nm10;
    m11 = nm11;
    m12 = nm12;
    m13 = nm13;
    m20 = nm20;
    m21 = nm21;
    m22 = nm22;
    m23 = nm23;
    m30 = nm30;
    m31 = nm31;
    m32 = nm32;
    m33 = nm33;
}

float Matrix4f::determinant() const
{
    // Cofactor expansion along the first row
    float s0 = m00 * (m11 * (m22 * m33 - m23 * m32) - m12 * (m21 * m33 - m23 * m31) + m13 * (m21 * m32 - m22 * m31));
    float s1 = m01 * (m10 * (m22 * m33 - m23 * m32) - m12 * (m20 * m33 - m23 * m30) + m13 * (m20 * m32 - m22 * m30));
    float s2 = m02 * (m10 * (m21 * m33 - m23 * m31) - m11 * (m20 * m33 - m23 * m30) + m13 * (m20 * m31 - m21 * m30));
    float s3 = m03 * (m10 * (m21 * m32 - m22 * m31) - m11 * (m20 * m32 - m22 * m30) + m12 * (m20 * m31 - m21 * m30));
    return s0 - s1 + s2 - s3;
}

void Matrix4f::invert()
{
    float s0 = m00 * m11 - m10 * m01;
    float s1 = m00 * m12 - m10 * m02;
    float s2 = m00 * m13 - m10 * m03;
    float s3 = m01 * m12 - m11 * m02;
    float s4 = m01 * m13 - m11 * m03;
    float s5 = m02 * m13 - m12 * m03;

    float c5 = m22 * m33 - m32 * m23;
    float c4 = m21 * m33 - m31 * m23;
    float c3 = m21 * m32 - m31 * m22;
    float c2 = m20 * m33 - m30 * m23;
    float c1 = m20 * m32 - m30 * m22;
    float c0 = m20 * m31 - m30 * m21;

    float det = (s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0);
    if (det == 0.0f)
        return; // Singular matrix

    float invDet = 1.0f / det;

    float nm00 = (m11 * c5 - m12 * c4 + m13 * c3) * invDet;
    float nm01 = (-m01 * c5 + m02 * c4 - m03 * c3) * invDet;
    float nm02 = (m31 * s5 - m32 * s4 + m33 * s3) * invDet;
    float nm03 = (-m21 * s5 + m22 * s4 - m23 * s3) * invDet;

    float nm10 = (-m10 * c5 + m12 * c2 - m13 * c1) * invDet;
    float nm11 = (m00 * c5 - m02 * c2 + m03 * c1) * invDet;
    float nm12 = (-m30 * s5 + m32 * s2 - m33 * s1) * invDet;
    float nm13 = (m20 * s5 - m22 * s2 + m23 * s1) * invDet;

    float nm20 = (m10 * c4 - m11 * c2 + m13 * c0) * invDet;
    float nm21 = (-m00 * c4 + m01 * c2 - m03 * c0) * invDet;
    float nm22 = (m30 * s4 - m31 * s2 + m33 * s0) * invDet;
    float nm23 = (-m20 * s4 + m21 * s2 - m23 * s0) * invDet;

    float nm30 = (-m10 * c3 + m11 * c1 - m12 * c0) * invDet;
    float nm31 = (m00 * c3 - m01 * c1 + m02 * c0) * invDet;
    float nm32 = (-m30 * s3 + m31 * s1 - m32 * s0) * invDet;
    float nm33 = (m20 * s3 - m21 * s1 + m22 * s0) * invDet;

    m00 = nm00;
    m01 = nm01;
    m02 = nm02;
    m03 = nm03;
    m10 = nm10;
    m11 = nm11;
    m12 = nm12;
    m13 = nm13;
    m20 = nm20;
    m21 = nm21;
    m22 = nm22;
    m23 = nm23;
    m30 = nm30;
    m31 = nm31;
    m32 = nm32;
    m33 = nm33;
}

void Matrix4f::transform(Point3f &point) const
{
    float x = m00 * point.x + m01 * point.y + m02 * point.z + m03;
    float y = m10 * point.x + m11 * point.y + m12 * point.z + m13;
    float z = m20 * point.x + m21 * point.y + m22 * point.z + m23;
    float w = m30 * point.x + m31 * point.y + m32 * point.z + m33;

    if (w != 1.0f && w != 0.0f)
    {
        point.x = x / w;
        point.y = y / w;
        point.z = z / w;
    }
    else
    {
        point.x = x;
        point.y = y;
        point.z = z;
    }
}

void Matrix4f::transform(Vector3f &normal) const
{
    float x = m00 * normal.x + m01 * normal.y + m02 * normal.z;
    float y = m10 * normal.x + m11 * normal.y + m12 * normal.z;
    float z = m20 * normal.x + m21 * normal.y + m22 * normal.z;

    normal.x = x;
    normal.y = y;
    normal.z = z;
}

void Matrix4f::setRotation(AxisAngle4f a1)
{
    float x = a1.x;
    float y = a1.y;
    float z = a1.z;
    float angle = a1.angle;

    // 1. Calculate the magnitude to normalize the axis
    float mag = std::sqrt(x * x + y * y + z * z);
    if (mag < 1e-6)
    {
        setIdentity();
        return;
    }

    x /= mag;
    y /= mag;
    z /= mag;

    float c = std::cos(angle);
    float s = std::sin(angle);
    float t = 1.0f - c;

    // m00, m01, m02,   0;
    // m10, m11, m12,   0;
    // m20, m21, m22,   0;
    //   0,   0,   0,   1;

    // 2. Build the rotation matrix (Column-Major)
    m00 = t * x * x + c;     // m[0]
    m10 = t * x * y + s * z; // m[1]
    m20 = t * x * z - s * y; // m[2]
    m30 = 0.0f;              // m[3]

    m01 = t * x * y - s * z; // m[4]
    m11 = t * y * y + c;     // m[5]
    m21 = t * y * z + s * x; // m[6]
    m31 = 0.0f;              // m[7]

    m02 = t * x * z + s * y; // m[8]
    m12 = t * y * z - s * x; // m[9]
    m22 = t * z * z + c;     // m[10]
    m32 = 0.0f;              // m[11]

    // 3. Set the last column for a pure rotation
    m03 = 0.0f; // m[12]
    m13 = 0.0f; // m[13]
    m23 = 0.0f; // m[14]
    m33 = 1.0f; // m[15]
}

std::string Matrix4f::toString() const
{
    std::stringstream ss;
    ss << m00 << ", " << m01 << ", " << m02 << ", " << m03 << "\n"
       << m10 << ", " << m11 << ", " << m12 << ", " << m13 << "\n"
       << m20 << ", " << m21 << ", " << m22 << ", " << m23 << "\n"
       << m30 << ", " << m31 << ", " << m32 << ", " << m33;
    return ss.str();
}

bool Matrix4f::equals(const Matrix4f &m1) const
{
    return m00 == m1.m00 && m01 == m1.m01 && m02 == m1.m02 && m03 == m1.m03 &&
           m10 == m1.m10 && m11 == m1.m11 && m12 == m1.m12 && m13 == m1.m13 &&
           m20 == m1.m20 && m21 == m1.m21 && m22 == m1.m22 && m23 == m1.m23 &&
           m30 == m1.m30 && m31 == m1.m31 && m32 == m1.m32 && m33 == m1.m33;
}

bool Matrix4f::epsilonEquals(const Matrix4f &m1, float epsilon) const
{
    return std::abs(m00 - m1.m00) < epsilon && std::abs(m01 - m1.m01) < epsilon &&
           std::abs(m02 - m1.m02) < epsilon && std::abs(m03 - m1.m03) < epsilon &&
           std::abs(m10 - m1.m10) < epsilon && std::abs(m11 - m1.m11) < epsilon &&
           std::abs(m12 - m1.m12) < epsilon && std::abs(m13 - m1.m13) < epsilon &&
           std::abs(m20 - m1.m20) < epsilon && std::abs(m21 - m1.m21) < epsilon &&
           std::abs(m22 - m1.m22) < epsilon && std::abs(m23 - m1.m23) < epsilon &&
           std::abs(m30 - m1.m30) < epsilon && std::abs(m31 - m1.m31) < epsilon &&
           std::abs(m32 - m1.m32) < epsilon && std::abs(m33 - m1.m33) < epsilon;
}