#pragma once
#include <iostream>
#include <string>

#include "Vector3f.h"
#include "Vector4f.h"

class Matrix4
{
private:
    float o[4][4]; // Results matrix

public:
    //      R  C
    //      o  o
    //      w  l
    float m[4][4];

    Matrix4(/* args */);
    ~Matrix4();

    void set(const Matrix4 &m);
    void setIdentity();
    void setZero();
    void setScale(float sx, float sy, float sz);
    void setTranslation(float tx, float ty, float tz);

    void setRotationX(float angle);
    void setRotationY(float angle);
    void setRotationZ(float angle);

    void multiply(Maths::Vector4f &out);
    void multiply(const Maths::Vector4f &in, Maths::Vector4f &out);
    void multiply(const Maths::Vector3f &v, Maths::Vector3f &out);
    void multiply(const Matrix4 &m1, const Matrix4 &m2);
    void multiply(const Matrix4 &m1, const Matrix4 &m2, Matrix4 &out);

    void makePerspective(float fov, float aspect, float znear, float zfar);

    void print() const;
    std::string toString() const;

    friend std::ostream &operator<<(std::ostream &os, const Matrix4 &m)
    {
        os << "|" << m.m[0][0] << ", " << m.m[0][1] << ", " << m.m[0][2] << ", " << m.m[0][3] << "|" << std::endl;
        os << "|" << m.m[1][0] << ", " << m.m[1][1] << ", " << m.m[1][2] << ", " << m.m[1][3] << "|" << std::endl;
        os << "|" << m.m[2][0] << ", " << m.m[2][1] << ", " << m.m[2][2] << ", " << m.m[2][3] << "|" << std::endl;
        os << "|" << m.m[3][0] << ", " << m.m[3][1] << ", " << m.m[3][2] << ", " << m.m[3][3] << "|" << std::endl;

        return os;
    }
};
