#pragma once

#include <string>
#include <ostream>

enum VectorType
{
    None,
    PointT,
    TextureT,
    Vector2T,
    Vector3T,
    Vector4T
};

/// @brief This class is used for Vector2f and Vector3f
class Vectorf
{
public:
    union
    {
        float x;
        float u;
    };
    union
    {
        float y;
        float v;
    };
    float z;
    float w;

    VectorType type{None};

    // Constructors
    Vectorf(const VectorType type = VectorType::Vector3T);
    Vectorf(float x, float y);
    Vectorf(float x, float y, float z);
    Vectorf(float x, float y, float z, float w);
    Vectorf(const Vectorf &v);
    ~Vectorf() = default;

    void setType(const VectorType type) { this->type = type; }
    void zero();

    // Setters
    void set(float x, float y);
    void set(float x, float y, float z);
    void set(float x, float y, float z, float w);
    void set(const Vectorf &v);
    void setAsTexture(float u, float v);
    // Conversions
    void set(const Vectorf &v, const VectorType fromType, const VectorType toType);
    void setFromT3ToT4(const Vectorf &v);
    void setFromTexture(Vectorf &t);

    void add(const Vectorf &v);
    void add(const Vectorf &v1, const Vectorf &v2);
    Vectorf addNew(const Vectorf &v);
    Vectorf addNew(const Vectorf &v1, const Vectorf &v2);

    void sub(const Vectorf &v);
    void sub(const Vectorf &v1, const Vectorf &v2);
    Vectorf subNew(const Vectorf &v);
    Vectorf subNew(const Vectorf &v1, const Vectorf &v2);

    void multiply(float s);
    void multiplyAdd(float s, const Vectorf &v1, const Vectorf &v2);

    void divide(float s);

    void negate();

    float length() const;
    float lengthSquared() const;
    void normalize();
    float dot(const Vectorf &v) const;
    friend float dot(const Vectorf &v1, const Vectorf &v2);
    void cross(const Vectorf &a, const Vectorf &b);

    bool equals(const Vectorf &v) const;
    bool epsilonEquals(const Vectorf &v, float epsilon) const;

    // Rotation Operators without using matrices
    Vectorf rotateX(float angle);
    Vectorf rotateY(float angle);
    Vectorf rotateZ(float angle);

    friend std::ostream &operator<<(std::ostream &os, const Vectorf &o)
    {
        switch (o.type)
        {
        case VectorType::None:
            return os << "None";
        case VectorType::PointT:
            return os << "Point: <" << o.x << ", " << o.y << ">";
            return os << "";
        case VectorType::Vector2T:
            return os << "<" << o.x << ", " << o.y << ">";
        case VectorType::Vector3T:
            return os << "<" << o.x << ", " << o.y << ", " << o.z << ">";
        case VectorType::Vector4T:
            return os << "<" << o.x << ", " << o.y << ", " << o.z << ", " << o.w << ">";
        }
    }
};