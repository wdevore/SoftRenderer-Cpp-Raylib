#include <cmath>

#include "Frustum.h"

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
    width = std::abs(right - left);
}

void Frustum::SetHeight(float top, float bottom)
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

void Frustum::BuildProjectionMatrix()
{
    volume.m00 = (2.0f * near) / (right - left);
    volume.m01 = 0.0f;
    volume.m02 = (right + left) / (right - left);
    volume.m03 = 0.0f;

    volume.m10 = 0.0f;
    volume.m11 = (2.0f * near) / (top - bottom);
    volume.m12 = (top + bottom) / (top - bottom);
    volume.m13 = 0.0f;

    volume.m20 = 0.0f;
    volume.m21 = 0.0f;
    volume.m22 = (far + near) / (near - far);
    volume.m23 = 2.0f * (far * near) / (near - far);

    volume.m30 = 0.0f;
    volume.m31 = 0.0f;
    volume.m32 = -1.0f;
    volume.m33 = 0.0f;
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
