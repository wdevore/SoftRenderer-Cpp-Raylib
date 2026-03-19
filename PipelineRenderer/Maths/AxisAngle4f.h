#pragma once
#include <string>

class AxisAngle4f
{
public:
    float x;
    float y;
    float z;
    float angle;

    AxisAngle4f();
    AxisAngle4f(float x, float y, float z, float angle);
    AxisAngle4f(const AxisAngle4f &a1);
    ~AxisAngle4f() = default;

    void set(float x, float y, float z, float angle);
    void set(const AxisAngle4f &a1);

    std::string toString() const;
    bool equals(const AxisAngle4f &a1) const;
    bool epsilonEquals(const AxisAngle4f &a1, float epsilon) const;

    // C++ specific operators
    bool operator==(const AxisAngle4f &other) const;
    bool operator!=(const AxisAngle4f &other) const;
};