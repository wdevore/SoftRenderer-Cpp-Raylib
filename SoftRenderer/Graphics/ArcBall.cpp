#include <iostream>

#include "ArcBall.h"

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
