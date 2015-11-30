/*
 * The PerspectiveCamera class derived from Camera
 * Implemented by Mohism Research
 */

#include "PerspectiveCamera.h"

#include <math.h>

#define PI 3.14159265

PerspectiveCamera::PerspectiveCamera(Vec3f &center, Vec3f &direction, Vec3f &up, float angle)
    : mCenter(center), mDirection(direction), mUp(up), mAngle(angle)
{
    float v = mUp.Dot3(mDirection);

    if (v != 0)
    {
        Vec3f assistant(0, 0, 1);
        Vec3f::Cross3(mUp, assistant, mDirection); 
    }

    if (mDirection.Length() != 1)
    {
        mDirection.Normalize();    
    }

    if (mUp.Length() != 1)
    {
        mUp.Normalize();    
    }

    Vec3f::Cross3(mHorizontal, mDirection, mUp);

    mRatio = 1.0;
}


Ray PerspectiveCamera::generateRay(Vec2f point)
{
    float x = point.x();
    float y = point.y();

    float height = 2 * tan(angle * PI / 360.0);
    float width = height * mRatio;

    float l = - width / 2.0;
    float t = - height / 2.0;

    float u = x * width + l;
    float v = y * height + t;

    float near = 1.0;
    Vec3f originalDir = near * mDirection + u * mHorizontal + v * mUp;

    if (originalDir.Length() != 0)
    {
        originalDir.Normalize();
    }

    Ray r(mCenter, originalDir);
    
    return r;
}
