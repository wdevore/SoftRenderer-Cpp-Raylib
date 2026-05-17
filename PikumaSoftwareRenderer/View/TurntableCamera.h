#pragma once

#include "Vector3f.h"
#include "Matrix4.h"

namespace View
{
    class TurntableCamera
    {
    public:
        Maths::Vector3f position{};
        Maths::Vector3f target{};
        Maths::Vector3f upDirection{0, 1, 0};

        float radius{10.0f};
        float yaw{0.0f};
        float pitch{0.0f};

        Maths::Vector3f desiredTarget{};
        float desiredRadius{10.0f};
        float desiredYaw{0.0f};
        float desiredPitch{0.0f};

        Matrix4 vm{}; // view matrix
        Matrix4 pm{}; // projection matrix

        TurntableCamera();
        ~TurntableCamera();

        void initialize(Maths::Vector3f target, float radius);

        void rotate(float deltaYaw, float deltaPitch);
        void zoom(float deltaRadius);
        void pan(float deltaX, float deltaY, const Maths::Vector3f &up);
        void update(float deltaTime);

        bool smoothControl{false};

        void updatePosition();

        void makeLookAt();
        void makePerspective(float fov, float aspect, float znear, float zfar);
    };
} // namespace View