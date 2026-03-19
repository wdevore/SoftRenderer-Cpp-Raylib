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
    nearPlane.SetPosition(px, py, pz);
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
    this->near = near;
    this->far = far;

    // Gemini version which uses the symmetric approach
    top = near * tanf(ffov * 0.5f * Maths::DEGTORAD);
    right = top * faspect;

    // Changing the sign of near inverts the mouse motion.
    // Note: This is the general projection.
    // right = -near * std::tan((ffov / 2.0f) * Maths::DEGTORAD);
    // left = -right;
    bottom = left / faspect;
    top = right / faspect;

    std::cout << "Frustum::SetPerspective distance " << (1.0f / std::tan(ffov)) << std::endl;

    Set(left, right, bottom, top, -near, -far);
}

void Frustum::BuildProjectionMatrix()
{
    // Gemini: A symmetric projection
    //          Col0  Col1  Col2   Col3
    //           v     v     v       v
    //          m0    m4    m8     m12
    // Row0 --> n/r   m01   m02    m03
    //          m1    m5    m9     m13
    // Row1 --> m10   n/t   m12    m13
    //          m2    m6    m10    m14
    // Row2 --> m20   m21   -f+n   -2fn
    //                      -----  -----
    //                      (f-n)  f-n)
    //          m3    m7    m11    m15
    // Row3 --> m30   m31   -1      0
    projection.setIdentity();

    // Col 0, Row 0
    projection.m00 = near / right; // m[0*4 + 0]

    // Col 1, Row 1
    projection.m11 = near / top; // m[1*4 + 1]

    // Col 2, Row 3 (The W-divide hook)
    projection.m23 = -1.0f; // m[2*4 + 3]

    // Col 3, Row 2 (The Z-offset)
    projection.m32 = -(2.0f * far * near) / (far - near); // m[3*4 + 2]

    // Col 3, Row 3
    projection.m33 = 0.0f; // m[3*4 + 3]
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
