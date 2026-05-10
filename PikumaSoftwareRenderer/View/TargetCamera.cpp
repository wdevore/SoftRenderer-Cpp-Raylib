#include <cmath>
#include <iostream>

#include "TargetCamera.h"
#include "Constants.h"

namespace View
{
    TargetCamera::TargetCamera()
    {
    }

    TargetCamera::~TargetCamera()
    {
    }

    void TargetCamera::initialize(Maths::Vector3f position)
    {
        this->position.set(position);
        forwardVelocity.zero();
    }

    void TargetCamera::setTarget(Maths::Vector3f newTarget)
    {
        target.set(newTarget);
    }

    void TargetCamera::updateVelocity(float speed)
    {
        direction.multiply(speed);
        forwardVelocity.set(direction);
    }

    void TargetCamera::updatePosition()
    {
        position.add(forwardVelocity);
    }

    void TargetCamera::setLookAtDirection()
    {
        // Target is explicitly set, not calculated from yaw/pitch.
        // Update the camera direction to point from the current position to the target.
        direction.sub(target, position);
        direction.normalize();
    }

    void TargetCamera::makeLookAt(const Maths::Vector3f &eye, const Maths::Vector3f &center, const Maths::Vector3f &up)
    {
        Maths::Vector3f x{}, y{}, z{};
        z.sub(center, eye);
        z.normalize();

        x.cross(up, z);
        x.normalize();

        y.cross(z, x);

        vm.m[0][0] = x.x;
        vm.m[0][1] = x.y;
        vm.m[0][2] = x.z;
        vm.m[0][3] = -x.dot(eye);
        vm.m[1][0] = y.x;
        vm.m[1][1] = y.y;
        vm.m[1][2] = y.z;
        vm.m[1][3] = -y.dot(eye);
        vm.m[2][0] = z.x;
        vm.m[2][1] = z.y;
        vm.m[2][2] = z.z;
        vm.m[2][3] = -z.dot(eye);
        vm.m[3][0] = 0.0;
        vm.m[3][1] = 0.0;
        vm.m[3][2] = 0.0;
        vm.m[3][3] = 1.0;
    }

    void TargetCamera::makePerspective(float fov, float aspect, float znear, float zfar)
    {
        pm.setZero();
        pm.m[0][0] = aspect * (1 / tan(fov / 2));
        pm.m[1][1] = 1 / tan(fov / 2);
        pm.m[2][2] = zfar / (zfar - znear);
        pm.m[2][3] = (-zfar * znear) / (zfar - znear);
        pm.m[3][2] = 1.0;
    }
} // namespace View