#include <cmath>
#include <iostream>

#include "TurntableCamera.h"
#include "Constants.h"

namespace View
{
    TurntableCamera::TurntableCamera() {}
    TurntableCamera::~TurntableCamera() {}

    void TurntableCamera::initialize(Maths::Vector3f target, float radius)
    {
        this->target.set(target);
        this->desiredTarget.set(target);
        this->radius = radius;
        this->desiredRadius = radius;
        this->yaw = 0.0f;
        this->desiredYaw = 0.0f;
        this->pitch = 0.0f;
        this->desiredPitch = 0.0f;
        updatePosition();
    }

    void TurntableCamera::rotate(float deltaYaw, float deltaPitch)
    {
        desiredYaw += deltaYaw;
        desiredPitch += deltaPitch;

        // Clamp pitch to prevent flipping / gimbal lock
        float pitchLimit = 89.0f * Maths::DEGTORAD;
        if (desiredPitch > pitchLimit)
            desiredPitch = pitchLimit;
        if (desiredPitch < -pitchLimit)
            desiredPitch = -pitchLimit;
    }

    void TurntableCamera::zoom(float deltaRadius)
    {
        desiredRadius -= deltaRadius;
        if (desiredRadius < 1.0f)
            desiredRadius = 1.0f; // Prevent moving through the target
    }

    void TurntableCamera::pan(float deltaX, float deltaY, const Maths::Vector3f &up)
    {
        Maths::Vector3f x{}, y{}, z{};

        // Forward vector is from position to target
        z.sub(target, position);
        z.normalize();

        // Calculate camera's local X and Y axes
        x.cross(up, z);
        x.normalize();
        y.cross(z, x);

        x.multiply(deltaX);
        y.multiply(deltaY);

        desiredTarget.add(x);
        desiredTarget.add(y);
    }

    void TurntableCamera::update(float deltaTime)
    {
        // Frame-rate independent exponential smoothing
        float lerpSpeed = 15.0f; // Lower is looser/heavier, Higher is snappier
        float t = 1.0f - std::exp(-lerpSpeed * deltaTime);

        yaw += (desiredYaw - yaw) * t;
        pitch += (desiredPitch - pitch) * t;
        radius += (desiredRadius - radius) * t;

        Maths::Vector3f targetDiff;
        targetDiff.sub(desiredTarget, target);
        targetDiff.multiply(t);
        target.add(targetDiff);

        updatePosition();
    }

    void TurntableCamera::updatePosition()
    {
        position.x = target.x + radius * std::cos(pitch) * std::sin(yaw);
        position.y = target.y + radius * std::sin(pitch);
        position.z = target.z + radius * std::cos(pitch) * std::cos(yaw);
    }

    void TurntableCamera::makeLookAt(const Maths::Vector3f &up)
    {
        Maths::Vector3f x{}, y{}, z{};

        // Forward vector is from position to target
        z.sub(target, position);
        z.normalize();

        // Right vector
        x.cross(up, z);
        x.normalize();

        // Recalculate up vector
        y.cross(z, x);

        vm.m[0][0] = x.x;
        vm.m[0][1] = x.y;
        vm.m[0][2] = x.z;
        vm.m[0][3] = -x.dot(position);
        vm.m[1][0] = y.x;
        vm.m[1][1] = y.y;
        vm.m[1][2] = y.z;
        vm.m[1][3] = -y.dot(position);
        vm.m[2][0] = z.x;
        vm.m[2][1] = z.y;
        vm.m[2][2] = z.z;
        vm.m[2][3] = -z.dot(position);
        vm.m[3][0] = 0.0;
        vm.m[3][1] = 0.0;
        vm.m[3][2] = 0.0;
        vm.m[3][3] = 1.0;
    }

    void TurntableCamera::makePerspective(float fov, float aspect, float znear, float zfar)
    {
        // (Code from your previous matrix configurations, omitted for brevity)
        pm.setZero();
        pm.m[0][0] = aspect * (1 / tan(fov / 2));
        pm.m[1][1] = 1 / tan(fov / 2);
        pm.m[2][2] = zfar / (zfar - znear);
        pm.m[2][3] = (-zfar * znear) / (zfar - znear);
        pm.m[3][2] = 1.0;
    }
} // namespace View