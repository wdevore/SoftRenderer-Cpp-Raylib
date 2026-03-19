#pragma once
#include <string>

class AxisAngle4f;
class Matrix4f;

class Quat4f
{
public:
    float x;
    float y;
    float z;
    float w;

    Quat4f();
    Quat4f(float x, float y, float z, float w);
    Quat4f(const Quat4f &q1);
    ~Quat4f() = default;

    void set(float x, float y, float z, float w);
    void set(const Quat4f &q1);
    void set(const AxisAngle4f &a1);
    void set(const Matrix4f &m1);

    void normalize();
    void conjugate();
    void inverse();

    void mul(const Quat4f &q1);
    void mul(const Quat4f &q1, const Quat4f &q2);
    void mulInverse(const Quat4f &q1, const Quat4f &q2);

    void interpolate(const Quat4f &q1, float alpha);
    void interpolate(const Quat4f &q1, const Quat4f &q2, float alpha);

    std::string toString() const;
    bool equals(const Quat4f &q1) const;
    bool epsilonEquals(const Quat4f &q1, float epsilon) const;
};