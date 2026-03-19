#include "Quat4f.h"
#include "AxisAngle4f.h"
#include "Matrix4f.h"
#include <cmath>
#include <sstream>

Quat4f::Quat4f() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

Quat4f::Quat4f(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

Quat4f::Quat4f(const Quat4f &q1) : x(q1.x), y(q1.y), z(q1.z), w(q1.w) {}

void Quat4f::set(float x, float y, float z, float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

void Quat4f::set(const Quat4f &q1)
{
    this->x = q1.x;
    this->y = q1.y;
    this->z = q1.z;
    this->w = q1.w;
}

void Quat4f::set(const AxisAngle4f &a1)
{
    float halfAngle = a1.angle * 0.5f;
    float sinHalf = std::sin(halfAngle);

    float mag = std::sqrt(a1.x * a1.x + a1.y * a1.y + a1.z * a1.z);
    if (mag < 1e-6f)
    {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
        w = 0.0f;
    }
    else
    {
        float invMag = 1.0f / mag;
        x = a1.x * invMag * sinHalf;
        y = a1.y * invMag * sinHalf;
        z = a1.z * invMag * sinHalf;
        w = std::cos(halfAngle);
    }
}

void Quat4f::set(const Matrix4f &m1)
{
    float tr = m1.m00 + m1.m11 + m1.m22;
    if (tr > 0.0f)
    {
        float s = std::sqrt(tr + 1.0f);
        w = s * 0.5f;
        s = 0.5f / s;
        x = (m1.m21 - m1.m12) * s;
        y = (m1.m02 - m1.m20) * s;
        z = (m1.m10 - m1.m01) * s;
    }
    else
    {
        if ((m1.m00 >= m1.m11) && (m1.m00 >= m1.m22))
        {
            float s = std::sqrt(1.0f + m1.m00 - m1.m11 - m1.m22);
            x = s * 0.5f;
            s = 0.5f / s;
            y = (m1.m10 + m1.m01) * s;
            z = (m1.m02 + m1.m20) * s;
            w = (m1.m21 - m1.m12) * s;
        }
        else if (m1.m11 > m1.m22)
        {
            float s = std::sqrt(1.0f + m1.m11 - m1.m00 - m1.m22);
            y = s * 0.5f;
            s = 0.5f / s;
            x = (m1.m10 + m1.m01) * s;
            z = (m1.m21 + m1.m12) * s;
            w = (m1.m02 - m1.m20) * s;
        }
        else
        {
            float s = std::sqrt(1.0f + m1.m22 - m1.m00 - m1.m11);
            z = s * 0.5f;
            s = 0.5f / s;
            x = (m1.m02 + m1.m20) * s;
            y = (m1.m21 + m1.m12) * s;
            w = (m1.m10 - m1.m01) * s;
        }
    }
}

void Quat4f::normalize()
{
    float norm = x * x + y * y + z * z + w * w;
    if (norm > 0.0f)
    {
        norm = 1.0f / std::sqrt(norm);
        x *= norm;
        y *= norm;
        z *= norm;
        w *= norm;
    }
    else
    {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
        w = 0.0f;
    }
}

void Quat4f::conjugate()
{
    x = -x;
    y = -y;
    z = -z;
}

void Quat4f::inverse()
{
    float norm = x * x + y * y + z * z + w * w;
    if (norm > 0.0f)
    {
        float invNorm = 1.0f / norm;
        x *= -invNorm;
        y *= -invNorm;
        z *= -invNorm;
        w *= invNorm;
    }
}

void Quat4f::mul(const Quat4f &q1)
{
    mul(*this, q1);
}

void Quat4f::mul(const Quat4f &q1, const Quat4f &q2)
{
    if (this == &q1 || this == &q2)
    {
        float tx, ty, tz, tw;
        tx = q1.x * q2.w + q1.w * q2.x + q1.y * q2.z - q1.z * q2.y;
        ty = q1.y * q2.w + q1.w * q2.y + q1.z * q2.x - q1.x * q2.z;
        tz = q1.z * q2.w + q1.w * q2.z + q1.x * q2.y - q1.y * q2.x;
        tw = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
        x = tx;
        y = ty;
        z = tz;
        w = tw;
    }
    else
    {
        x = q1.x * q2.w + q1.w * q2.x + q1.y * q2.z - q1.z * q2.y;
        y = q1.y * q2.w + q1.w * q2.y + q1.z * q2.x - q1.x * q2.z;
        z = q1.z * q2.w + q1.w * q2.z + q1.x * q2.y - q1.y * q2.x;
        w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
    }
}

void Quat4f::mulInverse(const Quat4f &q1, const Quat4f &q2)
{
    Quat4f tempQ2 = q2;
    tempQ2.inverse();
    mul(q1, tempQ2);
}

void Quat4f::interpolate(const Quat4f &q1, float alpha)
{
    float beta = 1.0f - alpha;
    x = beta * x + alpha * q1.x;
    y = beta * y + alpha * q1.y;
    z = beta * z + alpha * q1.z;
    w = beta * w + alpha * q1.w;
}

void Quat4f::interpolate(const Quat4f &q1, const Quat4f &q2, float alpha)
{
    float beta = 1.0f - alpha;
    x = beta * q1.x + alpha * q2.x;
    y = beta * q1.y + alpha * q2.y;
    z = beta * q1.z + alpha * q2.z;
    w = beta * q1.w + alpha * q2.w;
}

std::string Quat4f::toString() const
{
    std::stringstream ss;
    ss << "(" << x << ", " << y << ", " << z << ", " << w << ")";
    return ss.str();
}

bool Quat4f::equals(const Quat4f &q1) const
{
    return x == q1.x && y == q1.y && z == q1.z && w == q1.w;
}

bool Quat4f::epsilonEquals(const Quat4f &q1, float epsilon) const
{
    return std::abs(q1.x - x) < epsilon &&
           std::abs(q1.y - y) < epsilon &&
           std::abs(q1.z - z) < epsilon &&
           std::abs(q1.w - w) < epsilon;
}