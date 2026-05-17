#include <cmath>

#include "YawPitchCamera.h"
#include "Constants.h"

namespace View
{
    YawPitchCamera::YawPitchCamera(/* args */)
    {
    }

    YawPitchCamera::~YawPitchCamera()
    {
    }

    void YawPitchCamera::initialize(Maths::Vector3f position)
    {
        this->position.set(position);
        direction.set(0, 0, 1);
        forwardVelocity.zero();
        yaw = 0.0;
        pitch = 0.0;
    }

    /// @brief +Angle = CCW rotation, -Angle = CW rotation
    /// @param angle in Radians
    void YawPitchCamera::rotateYaw(float angle)
    {
        yaw += angle;
    }

    /// @brief +Angle = CCW rotation, -Angle = CW rotation
    /// @param angle in Radians
    void YawPitchCamera::rotatePitch(float angle)
    {
        pitch += angle;
    }

    void YawPitchCamera::updateVelocity(float speed)
    {
        direction.multiply(speed);
        forwardVelocity.set(direction);
    }

    void YawPitchCamera::updatePosition()
    {
        position.add(forwardVelocity);
    }

    Maths::Vector3f YawPitchCamera::getLookAtTarget()
    {
        // TODO: optimize away local objects
        // Initialize the target looking at the positive z-axis
        // Maths::Vector3f target = {0, 0, 1};
        target.set(0, 0, 1);

        Matrix4 yawRotation{};
        yawRotation.setRotationY(yaw);

        Matrix4 pitchRotation{};
        pitchRotation.setRotationX(pitch);

        // Create camera rotation matrix based on yaw and pitch
        // camera_rotation = camera_yaw_rotation x camera_pitch_rotation x Identity
        Matrix4 rotation{}; // Identity
        rotation.multiply(yawRotation, pitchRotation);

        // Update camera direction based on the rotation
        Maths::Vector3f camera_direction{};
        rotation.multiply(target, direction);
        // std::cout << "direction: " << direction << ",yaw: " << yaw * Maths::RADTODEG << ",pitch: " << pitch * Maths::RADTODEG << std::endl;

        // Offset the camera position in the direction where the camera is pointing at
        target.add(position, direction);

        return target;
    }

    void YawPitchCamera::makeLookAt(const Maths::Vector3f &eye, const Maths::Vector3f &target, const Maths::Vector3f &up)
    {
        // TODO: replace locals
        // Compute the forward (z), right (x), and up (y) vectors
        Maths::Vector3f x{}, y{}, z{};
        z.sub(target, eye);
        z.normalize();

        x.cross(up, z);
        x.normalize();

        y.cross(z, x);

        // | x.x   x.y   x.z  -dot(x,eye) |
        // | y.x   y.y   y.z  -dot(y,eye) |
        // | z.x   z.y   z.z  -dot(z,eye) |
        // |   0     0     0            1 |
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

    void YawPitchCamera::makePerspective(float fov, float aspect, float znear, float zfar)
    {
        // | (h/w)*1/tan(fov/2)             0              0                 0 |
        // |                  0  1/tan(fov/2)              0                 0 |
        // |                  0             0     zf/(zf-zn)  (-zf*zn)/(zf-zn) |
        // |                  0             0              1                 0 |
        pm.setZero();
        pm.m[0][0] = aspect * (1 / tan(fov / 2));
        pm.m[1][1] = 1 / tan(fov / 2);
        pm.m[2][2] = zfar / (zfar - znear);
        pm.m[2][3] = (-zfar * znear) / (zfar - znear);
        pm.m[3][2] = 1.0;
    }

}