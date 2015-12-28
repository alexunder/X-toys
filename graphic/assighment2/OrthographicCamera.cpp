/*
 * The implementation of the OrthographicCamera class
 * Implemented by Mohism Research
 */

#include "OrthographicCamera.h"

#define T_MIN -100000.0

OrthographicCamera::OrthographicCamera(const Vec3f &center, Vec3f &direction, Vec3f &up, int size)
    : mCenter(center), mDirection(direction), mUp(up), mCameraSize(size)
{
    float v = mUp.Dot3(mDirection);

    if (v != 0)
    {
        Vec3f temp;
        Vec3f::Cross3(temp, mDirection, mUp);
        Vec3f::Cross3(mUp, temp, mDirection);
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
#ifdef DEBUG
    cout<<"mCenter="<<mCenter<<endl;
    cout<<"mDirection="<<mDirection<<endl;
    cout<<"mUp="<<mUp<<endl;
    cout<<"mHorizontal="<<mHorizontal<<endl;
#endif
}

Ray OrthographicCamera::generateRay(Vec2f point)
{
    float x = point.x();
    float y = point.y();

    float l = - mCameraSize / 2.0;
    float t = - mCameraSize / 2.0;

    float u = x * mCameraSize + l;
    float v = y * mCameraSize + t;

    Vec3f originalPoint = mCenter + u * mHorizontal + v * mUp;
    Ray r(originalPoint, mDirection);

    return r;
}
    
float OrthographicCamera::getTMin() const
{
    return T_MIN;
}
