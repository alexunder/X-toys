/*
 * The implementation of the OrthographicCamera class
 * Implemented by Mohism Research
 */

#include "OrthographicCamera.h"

OrthographicCamera::OrthographicCamera(const Vec3f &center, Vec3f &direction, Vec3f &up, int size)
    : mCenter(center), mDirection(direction), mUp(up), mCameraSize(size)
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
}

Ray OrthographicCamera::generateRay(Vec2f point)
{
    float x = point.x();
    float y = point.y();

    float l = - mCameraSize / 2;
    float t = - mCameraSize / 2;

    float u = x * mCameraSize + l;
    float v = y * mCameraSize + t;

    Vec3f originalPoint = mCenter + u * mHorizontal + v * mUp;
    Ray r(originalPoint, mDirection);

    return r;
}
    
float OrthographicCamera::getTMin() const
{
    return 0.0;
}
