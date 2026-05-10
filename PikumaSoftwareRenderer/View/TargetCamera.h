#pragma once

#include "Vector3f.h"
#include "Matrix4.h"

namespace View
{
    class TargetCamera
    {
    public:
        Maths::Vector3f position{};
        Maths::Vector3f direction{};
        Maths::Vector3f forwardVelocity{};
        Maths::Vector3f target{};

        Matrix4 vm{}; // view matrix
        Matrix4 pm{}; // projection matrix

        TargetCamera();
        ~TargetCamera();

        void initialize(Maths::Vector3f position);
        void setTarget(Maths::Vector3f newTarget);

        void updateVelocity(float speed);
        void updatePosition();

        void setLookAtDirection();
        void makeLookAt(const Maths::Vector3f &eye, const Maths::Vector3f &center, const Maths::Vector3f &up);
        void makePerspective(float fov, float aspect, float znear, float zfar);
    };
} // namespace View