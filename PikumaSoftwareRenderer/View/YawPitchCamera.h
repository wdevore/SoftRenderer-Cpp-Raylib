#pragma once

#include "Vector3f.h"
#include "Matrix4.h"

namespace View
{
    class Camera
    {
    private:
        /* data */
    public:
        Maths::Vector3f position{};
        Maths::Vector3f direction{};
        Maths::Vector3f forwardVelocity{};
        float yaw;
        float pitch;
        Maths::Vector3f target{};

        Matrix4 vm{}; // view matrix
        Matrix4 pm{}; // projection matrix

        Camera(/* args */);
        ~Camera();

        void initialize(Maths::Vector3f position);

        void rotateYaw(float angle);
        void rotatePitch(float angle);
        void updateVelocity(float speed);
        void updatePosition();

        Maths::Vector3f getLookAtTarget();
        void makeLookAt(const Maths::Vector3f &eye, const Maths::Vector3f &center, const Maths::Vector3f &up);
        void makePerspective(float fov, float aspect, float znear, float zfar);
    };
} // namespace View
