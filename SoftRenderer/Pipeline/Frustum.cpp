#include <cmath>
#include <iostream>

#include "Frustum.h"
#include "Constants.h"

Frustum::Frustum(/* args */)
{
}

Frustum::~Frustum()
{
}

void Frustum::SetWidth(float left, float right)
{
    this->left = left;
    this->right = right;
}

void Frustum::SetHeight(float top, float bottom)
{
    this->top = top;
    this->bottom = bottom;
}

void Frustum::SetDepth(float near, float far)
{
    this->near = near;
    this->far = far;
    depth = std::abs(near - far);
}

void Frustum::SetNearPlane(float nx, float ny, float nz, float px, float py, float pz)
{
    nearPlane.SetNormal(nx, ny, nz);
    nearPlane.SetPoint(px, py, pz);
}

void Frustum::Set(float left, float right, float bottom, float top, float near, float far)
{
    if (near == far || left == right || bottom == top)
        return;

    SetWidth(left, right);
    SetHeight(top, bottom);
    SetDepth(near, far);

    BuildProjectionMatrix();

    // The near plane's frontside is determined by its normal.
    SetNearPlane(nearPlaneNormal.x, nearPlaneNormal.y, nearPlaneNormal.z, 0.0f, 0.0f, 1.0f);
}

void Frustum::SetPerspective(float ffov, float faspect, float near, float far)
{
    double left, right, bottom, top;

    // Changing the sign of near inverts the mouse motion.
    right = -near * std::tan((ffov / 2.0f) * (Maths::PI / 180.0f));
    left = -right;
    bottom = left / faspect;
    top = right / faspect;

    std::cout << "Frustum::SetPerspective distance " << (1.0f / std::tan(ffov)) << std::endl;

    Set(left, right, bottom, top, -near, -far);
}

void Frustum::BuildProjectionMatrix()
{
    projection.m00 = (2.0f * near) / (right - left);
    projection.m01 = 0.0f;
    projection.m02 = (right + left) / (right - left);
    projection.m03 = 0.0f;

    projection.m10 = 0.0f;
    projection.m11 = (2.0f * near) / (top - bottom);
    projection.m12 = (top + bottom) / (top - bottom);
    projection.m13 = 0.0f;

    projection.m20 = 0.0f;
    projection.m21 = 0.0f;
    projection.m22 = (far + near) / (near - far);
    projection.m23 = 2.0f * (far * near) / (near - far);

    projection.m30 = 0.0f;
    projection.m31 = 0.0f;
    projection.m32 = -1.0f;
    projection.m33 = 0.0f;
}

/**
 * <p>
 * The camera is located at 0,0,1 after view-volume matrix is applied.
 * The Near plane is located somewhere greater than 0,0,0 along the -z-axis.
 * For example, if near = -2 then the distance is 1 + abs(-2) = 3.
 * This means the base/eye of the camera is 3 units from the near plane.
 * </p>
 * <p>
 * This distance is required when performing backface culling. If the eye
 * is too close or too far then the culling culls early or late.
 * </p>
 * @return
 * The absolute distance from the near plane to camera/eye base.
 */

/// @brief
/// The camera is located at 0,0,1 after view-volume matrix is applied.
/// The Near plane is located somewhere greater than 0,0,0 along the -z-axis.
/// For example, if near = -2 then the distance is 1 + abs(-2) = 3.
/// This means the base/eye of the camera is 3 units from the near plane.
///
/// This distance is required when performing backface culling. If the eye
/// is too close or too far then the culling culls early or late.
/// @return
/// The absolute distance from the near plane to camera/eye base.
float Frustum::GetViewDistance()
{
    return 1.0f + std::abs(near);
}
