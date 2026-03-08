#include <iostream>
#include <cmath>

#include "ArcBall.h"
#include "Constants.h"

ArcBall::ArcBall(/* args */)
{
}

ArcBall::~ArcBall()
{
}

void ArcBall::initialize()
{
    pts.resize(NSEGS + 1);
    reset();
}

void ArcBall::reset()
{
    vCanvasCenter.set(0.0f, 0.0f, 0.0f);
    fRadius = 0.0f;
    vDown.set(0.0f, 0.0f, 0.0f);
    vNow.set(0.0f, 0.0f, 0.0f);
    qDown.set(0.0f, 0.0f, 0.0f, 1.0f);
    qNow.set(0.0f, 0.0f, 0.0f, 1.0f);

    vBase.set(0.0f, 0.0f, 0.0f); // GL default base location. world origin.

    affineTransform.setIdentity();
    rotation.setIdentity();
    translation.setIdentity();
}

void ArcBall::Place(Vector3f v, float r)
{
    vCanvasCenter.set(v);
    fRadius = r;
}

void ArcBall::Resize(int width, int height)
{
    this->width = width;
    this->height = height;
    vCanvasCenter.x = width / 2.0f;
    vCanvasCenter.y = height / 2.0f;
    // 2.5f gives a decent size ball. 2.0f would give little room on the sides.
    fRadius = std::min(width, height) / 2.5f;
}

void ArcBall::MapScreenCoords(int x, int y)
{
    if (bScreenYOrientation)
        vNow.set(x, height - y, 0.0f);
    else
        vNow.set(x, y, 0.0f);
}

void ArcBall::Update()
{
    MapMouseToSphere(vNow);
    vTo.set(vBallMouse);

    if (bDragging)
    {
        MapFromBallPoints(vFrom, vTo);
        qDrag.set(vBallPoint);
        qNow.mul(qDrag, qDown);
    }
    MapToBallPoints(qDown, vrFrom, vrTo);
    q.set(qNow);
    // mNow.set(q); // Optional if using Matrix.
}

void ArcBall::MapMouseToSphere(const Vector3f &vMouse)
{
    vBallMouse.x = (vMouse.x - vCanvasCenter.x) / fRadius;
    vBallMouse.y = (vMouse.y - vCanvasCenter.y) / fRadius;
    vBallMouse.z = 0.0f;
    float mag = vBallMouse.lengthSquared();
    if (mag > 1.0)
    {
        float scale = 1.0f / (float)std::sqrt(mag);
        vBallMouse.scale(scale);
    }
    else
    {
        vBallMouse.z = (float)std::sqrt(1.0f - mag);
    }
}

void ArcBall::MapFromBallPoints(const Vector3f &vFrom, const Vector3f &vTo)
{
    vVector.cross(vFrom, vTo);
    vBallPoint.set(vVector.x, vVector.y, vVector.z, vFrom.dot(vTo));
}

void ArcBall::MapToBallPoints(const Quat4f &q, Vector3f &vArcFrom, Vector3f &vArcTo)
{
    float s = (float)std::sqrt(q.x * q.x + q.y * q.y);
    if (s == 0.0f)
    {
        vArcFrom.set(0.0f, 1.0f, 0.0f);
    }
    else
    {
        vArcFrom.set(-q.y / s, q.x / s, 0.0f);
    }
    vArcTo.x = q.w * vArcFrom.x - q.z * vArcFrom.y;
    vArcTo.y = q.w * vArcFrom.y + q.z * vArcFrom.x;
    vArcTo.z = q.x * vArcFrom.y - q.y * vArcFrom.x;
    if (q.w < 0.0)
        vArcFrom.set(-vArcFrom.x, -vArcFrom.y, 0.0f);
}

void ArcBall::MouseOnSphere(const Vector3f &vMouse)
{
    vBallMouse.x = (vMouse.x - vCanvasCenter.x) / fRadius;
    vBallMouse.y = (vMouse.y - vCanvasCenter.y) / fRadius;
    vBallMouse.z = 0.0f;
    float mag = vBallMouse.lengthSquared();
    if (mag > 1.0)
    {
        float scale = 1.0f / (float)std::sqrt(mag);
        vBallMouse.scale(scale);
    }
    else
    {
        vBallMouse.z = (float)std::sqrt(1.0f - mag);
    }
}

void ArcBall::LookInDirection(const Vector3f &vDir)
{
    // vVector is the default GL camera direction = -Z axis
    // vNow is the axis of rotation = cross product of vDir into (-Z axis)
    //
    // Remember the cross product is not cummunative so (-Z axis) into vDir
    // produces the opposite result, not good. We are using Ken's system so
    // I try to be consistent hence I use the same system that GL uses and
    // that is a right-handed system. Your fingers curl in the direction of
    // the cross product and your thumb is the resultant normal vector.
    // The dot product is communative so its order dose not matter.
    //
    // Hence, qNow = [(vDir.cross(-Z axis)), 1.0 + (-Z axis).dot(vDir)]
    vDirection.set(vDir);                // remember the direction for later use.
    vDirection.normalize();              // make sure it is of unit length.
    vVector.set(0.0f, 0.0f, -1.0f);      // GL default camera direction
    vNow.cross(vDirection, vVector);     // get the axis of rotation
    float dot = vVector.dot(vDirection); // and how much we will rotate by.

    // Check to see if given direction is pointing is the exact opposite of the
    // camera direction.
    // In GL the camera is always sitting at the world origin looking down the -Z
    // axis.
    //
    // The dot product tells the angle between the direction and camera and is in
    // the range of
    // -1 to 0 to 1. 0 meaning they are perpindicular, -1 meaning they are pointing
    // in opposite
    // directions.
    //
    // We are only concerned with a -1 dot product or at least within an Epsilon of
    // it.
    // We will get a -1 when the direction is on or close to the +Z axis. The dot
    // product of
    // -Z and +Z gives -1.
    //
    // The arcball fails if the desired viewing direction and camera direction are
    // pointing in opposite
    // directions. Why? Because there is an infinite numbers of rotations that would
    // satifiy the
    // request. The resultent quaternion would be undefined. So instead, a manual
    // rotation is
    // performed about the +y axis as shown below.
    float delta = 1.0f - std::abs(dot);
    if (delta < Maths::EPSILON && dot < 0.0f)
    {
        // Close enough
        qNow.set(aaYaxis);
    }
    else
    {
        qNow.set(vNow.x, vNow.y, vNow.z, -(1.0f + dot));
    }

    qNow.normalize();

    qDown.set(qNow);
}

void ArcBall::LookAt(const Vector3f &vTarget)
{
    // vBase is typically the camera's base location. In trivial examples
    // the base is located at the world origin so the target is pretty much
    // the direction normalized.
    vDirection.sub(vTarget, vBase);
    LookInDirection(vDirection);

    vLos.set(vDirection);

    Vector3f worldUp(0.0f, 1.0f, 0.0f);
    vHorz.cross(vLos, worldUp);
    vHorz.normalize();

    vUp.cross(vHorz, vLos);
    vUp.normalize();
}

void ArcBall::LookAt(const Vector3f &vBase, const Vector3f &vTarget)
{
    // If the camera is located somewhere other than the world origin then
    // the direction is = (target - base) normalized.
    this->vBase.set(vBase);
    LookAt(vTarget);
}

void ArcBall::LookAt(float xb, float yb, float zb, float xt, float yt, float zt)
{
    // If the camera is located somewhere other than the world origin then
    // the direction is = (target - base) normalized.
    this->vBase.set(xb, yb, zb);
    vTarget.set(xt, yt, zt);
    LookAt(vTarget);
}

void ArcBall::ComputeLOS()
{
    qConj.set(qNow);
    qConj.conjugate();
    q.set(0.0f, 0.0f, -1.0f, 0.0f); // -Z
    q.mul(qConj, q);
    q.mul(qNow);
    vLos.set(q.x, q.y, q.z); // the camera's los
    vLos.normalize();
}

Matrix4f &ArcBall::GetTransformMatrix4f()
{
    // We want a camera that pans and not orbits which means we
    // need to control the order of the multiplication which means
    // we need two matrices; one for rotation and the other for
    // translation.
    //
    // If the screen coordinates where reverse we would use the conjugate.
    // qConj.conjugate(qNow);
    rotation.setIdentity();
    rotation.setRotation(qNow);

    translation.setIdentity();
    vVector.set(vBase);
    vVector.negate();
    translation.setTranslation(vVector);

    // Note the order in which I perform the multiplication. I do a
    // rotation followed by a translation which will create a panning effect.
    //
    // To have an orbiting effect I would do a translation first followed by
    // a rotation. Note: post multiply requires TxR not RxT.
    affineTransform.setIdentity();
    affineTransform.mul(translation, rotation);

    return affineTransform;
}

Point3f &ArcBall::GetWorldPosition()
{
    p1.set(vBase);
    GetTransformMatrix4f();
    affineTransform.invert();
    affineTransform.transform(p1, p2);
    return p2;
}

void ArcBall::MoveCameraBase(float dx, float dy, float dz)
{
    // Modify camera's base vector based on local-space horizontal normal vector.
    vBase.set(vBase.x + (dx * vHorz.x), vBase.y + (dy * vUp.y), vBase.z + (dz * vLos.z));
    // std::cout << "Base: " << vBase.x << ", " << vBase.y << ", " << vBase.z << std::endl;
    LookAt(vTarget);
}

void ArcBall::OnMouseDown(int x, int y)
{
    MapScreenCoords(x, y);
    MapMouseToSphere(vNow);
    vFrom.set(vBallMouse);
    qDown.set(qNow);
    bDragging = true;
}

void ArcBall::OnMouseUp() { bDragging = false; }

void ArcBall::OnMouseMove(int x, int y)
{
    MapScreenCoords(x, y);
}
