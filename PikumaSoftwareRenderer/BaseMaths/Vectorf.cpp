#include <cmath>
#include <sstream>

#include "Vectorf.h"

Vectorf::Vectorf(const VectorType type) : type(type)
{
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    w = 0.0f;
}

Vectorf::Vectorf(float x, float y)
{
    this->x = x;
    this->y = y;
    type = Vector2T;
}

Vectorf::Vectorf(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
    type = Vector3T;
}

Vectorf::Vectorf(float x, float y, float z, float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
    type = Vector4T;
}

Vectorf::Vectorf(const Vectorf &v1) : type(v1.type)
{
    x = v1.x;
    y = v1.y;
    z = v1.z;
    w = v1.w;
}

void Vectorf::zero()
{
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    w = 0.0f;
}

/// @brief A Point where `w` = 0.0f
/// @param x
/// @param y
void Vectorf::set(float x, float y)
{
    this->x = x;
    this->y = y;
    z = 0.0f;
    w = 0.0f;
}

void Vectorf::set(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
    w = 1.0f;
}

void Vectorf::set(float x, float y, float z, float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

void Vectorf::set(const Vectorf &v)
{
    set(v, v.type, this->type);
}

void Vectorf::setAsTexture(float u, float v)
{
    this->u = u;
    this->v = v;
    type = TextureT;
}

void Vectorf::set(const Vectorf &v, const VectorType fromType, const VectorType toType)
{
    this->type = toType;

    if (fromType == VectorType::Vector3T && toType == VectorType::Vector4T)
    {
        // Convert from Vector3 to Vector4
        x = v.x;
        y = v.y;
        z = v.z;
        w = 1.0f;
        return;
    }
    if (fromType == VectorType::Vector4T && toType == VectorType::Vector3T)
    {
        // Convert from Vector4 to Vector3
        x = v.x;
        y = v.y;
        z = v.z;
        return;
    }
    if (fromType == VectorType::Vector4T && toType == VectorType::Vector2T)
    {
        // Convert from Vector4 to Vector2
        x = v.x;
        y = v.y;
        return;
    }
}

void Vectorf::setFromT3ToT4(const Vectorf &v)
{
    type = VectorType::Vector4T;
    x = v.x;
    y = v.y;
    z = v.z;
    w = 1.0f;
}

void Vectorf::setFromTexture(Vectorf &t)
{
    type = VectorType::TextureT;
    x = t.u;
    y = t.v;
}

void Vectorf::add(const Vectorf &v)
{
    switch (type)
    {
    case VectorType::Vector2T:
        x += v.x;
        y += v.y;
        z = 0.0f;
        w = 0.0f;
        break;
    case VectorType::Vector3T:
        x += v.x;
        y += v.y;
        z += v.z;
        w = 1.0f;
        break;
    }
}

void Vectorf::add(const Vectorf &v1, const Vectorf &v2)
{
    switch (type)
    {
    case VectorType::Vector2T:
        x = v1.x + v2.x;
        y = v1.y + v2.y;
        z = 0.0f;
        w = 0.0f;
        break;
    case VectorType::Vector3T:
        x = v1.x + v2.x;
        y = v1.y + v2.y;
        z = v1.z + v2.z;
        w = 1.0f;
        break;
    }
}

Vectorf Vectorf::addNew(const Vectorf &v)
{
    switch (type)
    {
    case VectorType::Vector2T:
        return Vectorf{x + v.x, y + v.y, 0.0f, 0.0f};
        break;
    case VectorType::Vector3T:
        return Vectorf{x + v.x, y + v.y, z + v.z, 1.0f};
        break;
    }
    return Vectorf{};
}

Vectorf Vectorf::addNew(const Vectorf &v1, const Vectorf &v2)
{
    switch (type)
    {
    case VectorType::Vector2T:
        return Vectorf{v1.x + v2.x, v1.y + v2.y, 0.0f, 0.0f};
        break;
    case VectorType::Vector3T:
        return Vectorf{v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, 1.0f};
        break;
    }
    return Vectorf{};
}

void Vectorf::sub(const Vectorf &v)
{
    switch (type)
    {
    case VectorType::Vector2T:
        x -= v.x;
        y -= v.y;
        z = 0.0f;
        w = 0.0f;
        break;
    case VectorType::Vector3T:
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w = 1.0f;
        break;
    }
}

void Vectorf::sub(const Vectorf &v1, const Vectorf &v2)
{
    switch (type)
    {
    case VectorType::Vector2T:
        x = v1.x - v2.x;
        y = v1.y - v2.y;
        z = 0.0f;
        w = 0.0f;
        break;
    case VectorType::Vector3T:
        x = v1.x - v2.x;
        y = v1.y - v2.y;
        z = v1.z - v2.z;
        w = 1.0f;
        break;
    }
}

Vectorf Vectorf::subNew(const Vectorf &v)
{
    switch (type)
    {
    case VectorType::Vector2T:
        return Vectorf{x - v.x, y - v.y, 0.0f, 0.0f};
        break;
    case VectorType::Vector3T:
        return Vectorf{x - v.x, y - v.y, z - v.z, 1.0f};
        break;
    }
    return Vectorf{};
}

Vectorf Vectorf::subNew(const Vectorf &v1, const Vectorf &v2)
{
    switch (type)
    {
    case VectorType::Vector2T:
        return Vectorf{v1.x - v2.x, v1.y - v2.y, 0.0f, 0.0f};
        break;
    case VectorType::Vector3T:
        return Vectorf{v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, 1.0f};
        break;
    }
    return Vectorf{};
}

void Vectorf::multiply(float s)
{
    switch (type)
    {
    case VectorType::Vector2T:
        x *= s;
        y *= s;
        z = 0.0f;
        w = 0.0f;
        break;
    case VectorType::Vector3T:
        x *= s;
        y *= s;
        z *= s;
        w = 1.0f;
        break;
    }
}

void Vectorf::multiplyAdd(float s, const Vectorf &v1, const Vectorf &v2)
{
    switch (type)
    {
    case VectorType::Vector2T:
        x = s * v1.x + v2.x;
        y = s * v1.y + v2.y;
        z = 0.0f;
        w = 0.0f;
        break;

    case VectorType::Vector3T:
        x = s * v1.x + v2.x;
        y = s * v1.y + v2.y;
        z = s * v1.z + v2.z;
        w = 1.0f;
        break;
    }
}

void Vectorf::divide(float s)
{
    if (s == 0.0f)
        return;

    switch (type)
    {
    case VectorType::Vector2T:
        x /= s;
        y /= s;
        z = 0.0f;
        w = 0.0f;
        break;
    case VectorType::Vector3T:
        x /= s;
        y /= s;
        z /= s;
        w = 1.0f;
        break;
    }
}

void Vectorf::negate()
{
    switch (type)
    {
    case VectorType::Vector2T:
        x = -x;
        y = -y;
        z = -z;
        w = 0.0f;
        break;
    case VectorType::Vector3T:
        x = -x;
        y = -y;
        z = -z;
        w = 1.0f;
        break;
    }
}

float Vectorf::length() const
{
    switch (type)
    {
    case VectorType::Vector2T:
        return std::sqrt(x * x + y * y);
    case VectorType::Vector3T:
        return std::sqrt(x * x + y * y + z * z);
        break;
    default:
        return 0.0f;
        break;
    }
}

float Vectorf::lengthSquared() const
{
    switch (type)
    {
    case VectorType::Vector2T:
        return x * x + y * y;
    case VectorType::Vector3T:
        return x * x + y * y + z * z;
        break;
    default:
        return 0.0f;
        break;
    }
}

void Vectorf::normalize()
{
    float l = length();
    if (l != 0.0f)
    {
        x /= l;
        y /= l;
        z /= l;
    }
}

float Vectorf::dot(const Vectorf &v) const
{
    switch (type)
    {
    case VectorType::Vector2T:
        return x * v.x + y * v.y;
    case VectorType::Vector3T:
        return x * v.x + y * v.y + z * v.z;
    }
    return 0.0f;
}

void Vectorf::cross(const Vectorf &a, const Vectorf &b)
{
    x = a.y * b.z - a.z * b.y;
    y = a.z * b.x - a.x * b.z;
    z = a.x * b.y - a.y * b.x;
}

bool Vectorf::equals(const Vectorf &v) const
{
    return x == v.x && y == v.y && z == v.z;
}

bool Vectorf::epsilonEquals(const Vectorf &v, float epsilon) const
{
    return std::abs(v.x - x) < epsilon &&
           std::abs(v.y - y) < epsilon &&
           std::abs(v.z - z) < epsilon;
}

Vectorf Vectorf::rotateX(float angle)
{
    float s = std::sin(angle);
    float c = std::cos(angle);
    return Vectorf{
        x,
        this->y * c - this->z * s,
        this->y * s + this->z * c};
}

Vectorf Vectorf::rotateY(float angle)
{
    float s = std::sin(angle);
    float c = std::cos(angle);
    return Vectorf{
        this->x * c + this->z * s,
        y,
        this->x * s + this->z * c};
}

Vectorf Vectorf::rotateZ(float angle)
{
    float s = std::sin(angle);
    float c = std::cos(angle);
    return Vectorf{
        this->x * c - this->y * s,
        this->x * s + this->y * c,
        z};
}

float dot(const Vectorf &v1, const Vectorf &v2)
{
    switch (v1.type)
    {
    case VectorType::Vector2T:
        return v1.x * v2.x + v1.y * v2.y;
    case VectorType::Vector3T:
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }
    return 0.0f;
}
