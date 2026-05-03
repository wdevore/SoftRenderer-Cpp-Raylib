#include <cmath>
#include <iostream>
#include <sstream>

#include "Matrix4.h"

Matrix4::Matrix4(/* args */)
{
    setIdentity();
}

Matrix4::~Matrix4()
{
}

void Matrix4::set(const Matrix4 &m)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            this->m[i][j] = m.m[i][j];
        }
    }
}

void Matrix4::setIdentity()
{
    // | 1 0 0 0 |
    // | 0 1 0 0 |
    // | 0 0 1 0 |
    // | 0 0 0 1 |
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            m[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}

void Matrix4::setZero()
{
    // | 0 0 0 0 |
    // | 0 0 0 0 |
    // | 0 0 0 0 |
    // | 0 0 0 0 |
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            m[i][j] = 0.0f;
        }
    }
}

void Matrix4::setScale(float sx, float sy, float sz)
{
    // | sx  0  0  0 |
    // |  0 sy  0  0 |
    // |  0  0 sz  0 |
    // |  0  0  0  1 |

    setIdentity();
    m[0][0] = sx;
    m[1][1] = sy;
    m[2][2] = sz;
}

void Matrix4::setTranslation(float tx, float ty, float tz)
{
    // | 1  0  0  tx |
    // | 0  1  0  ty |
    // | 0  0  1  tz |
    // | 0  0  0  1  |
    setIdentity();
    m[0][3] = tx;
    m[1][3] = ty;
    m[2][3] = tz;
}

void Matrix4::setRotationX(float angle)
{
    float c = std::cos(angle);
    float s = std::sin(angle);
    // | 1  0  0  0 |
    // | 0  c -s  0 |
    // | 0  s  c  0 |
    // | 0  0  0  1 |
    setIdentity();
    m[1][1] = c;
    m[1][2] = -s;
    m[2][1] = s;
    m[2][2] = c;
}

void Matrix4::setRotationY(float angle)
{
    float c = std::cos(angle);
    float s = std::sin(angle);
    // |  c  0  s  0 |
    // |  0  1  0  0 |
    // | -s  0  c  0 |
    // |  0  0  0  1 |
    setIdentity();
    m[0][0] = c;
    m[0][2] = s;
    m[2][0] = -s;
    m[2][2] = c;
}

void Matrix4::setRotationZ(float angle)
{
    float c = std::cos(angle);
    float s = std::sin(angle);
    // | c -s  0  0 |
    // | s  c  0  0 |
    // | 0  0  1  0 |
    // | 0  0  0  1 |
    setIdentity();
    m[0][0] = c;
    m[0][1] = -s;
    m[1][0] = s;
    m[1][1] = c;
}

void Matrix4::multiply(Maths::Vector4f &out)
{
    Maths::Vector3f o{};
    o.x = m[0][0] * out.x + m[0][1] * out.y + m[0][2] * out.z + m[0][3] * out.w;
    o.y = m[1][0] * out.x + m[1][1] * out.y + m[1][2] * out.z + m[1][3] * out.w;
    o.z = m[2][0] * out.x + m[2][1] * out.y + m[2][2] * out.z + m[2][3] * out.w;
    o.w = m[3][0] * out.x + m[3][1] * out.y + m[3][2] * out.z + m[3][3] * out.w;
    out.set(o);
}

void Matrix4::multiply(const Maths::Vector4f &in, Maths::Vector4f &out)
{
    Maths::Vector3f o{};
    o.x = m[0][0] * in.x + m[0][1] * in.y + m[0][2] * in.z + m[0][3] * in.w;
    o.y = m[1][0] * in.x + m[1][1] * in.y + m[1][2] * in.z + m[1][3] * in.w;
    o.z = m[2][0] * in.x + m[2][1] * in.y + m[2][2] * in.z + m[2][3] * in.w;
    o.w = m[3][0] * in.x + m[3][1] * in.y + m[3][2] * in.z + m[3][3] * in.w;
    out.set(o);
}

void Matrix4::multiply(const Maths::Vector3f &v, Maths::Vector3f &out)
{
    out.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w;
    out.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w;
    out.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w;
    out.w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w;
}

void Matrix4::multiply(const Matrix4 &m1, const Matrix4 &m2)
{
    // TODO: unroll loops
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            o[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] + m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
        }
    }

    m[0][0] = o[0][0];
    m[1][0] = o[1][0];
    m[2][0] = o[2][0];
    m[3][0] = o[3][0];

    m[0][1] = o[0][1];
    m[1][1] = o[1][1];
    m[2][1] = o[2][1];
    m[3][1] = o[3][1];

    m[0][2] = o[0][2];
    m[1][2] = o[1][2];
    m[2][2] = o[2][2];
    m[3][2] = o[3][2];

    m[0][3] = o[0][3];
    m[1][3] = o[1][3];
    m[2][3] = o[2][3];
    m[3][3] = o[3][3];
}

/// @brief
/// @param m1
/// @param m2
/// @param out should **NOT** be a reference to either `m1` or `m2`
void Matrix4::multiply(const Matrix4 &m1, const Matrix4 &m2, Matrix4 &out)
{
    // TODO: unroll loops
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            out.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] + m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
        }
    }
}

void Matrix4::makePerspective(float fov, float aspect, float znear, float zfar)
{
    // | (h/w)*1/tan(fov/2)             0              0                 0 |
    // |                  0  1/tan(fov/2)              0                 0 |
    // |                  0             0     zf/(zf-zn)  (-zf*zn)/(zf-zn) |
    // |                  0             0              1                 0 |
    setZero();
    m[0][0] = aspect * (1 / tan(fov / 2));
    m[1][1] = 1 / tan(fov / 2);
    m[2][2] = zfar / (zfar - znear);
    m[2][3] = (-zfar * znear) / (zfar - znear);
    m[3][2] = 1.0;
}

void Matrix4::print() const
{
    std::cout << *this << std::endl;
}

std::string Matrix4::toString() const
{
    std::ostringstream oss;
    oss << *this;

    return oss.str();
}
