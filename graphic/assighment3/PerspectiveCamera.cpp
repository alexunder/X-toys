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

    mRatio = 1.0;
#ifdef DEBUG
    printf("mAngle=%f\n", mAngle);
#endif
}


Ray PerspectiveCamera::generateRay(Vec2f point)
{
    float x_ndc = point.x();
    float y_ndc = point.y();
#ifdef DEBUG
    printf("PerspectiveCamera::generateRay, x_ndc=%f, y_ndc=%f\n", x_ndc, y_ndc);
#endif
    float screenWidth = 0.f;
    float screenHeight = 0.f;

    if (mRatio > 1.f)
    {
        screenWidth = 2 * mRatio;
        screenHeight = 2.f;
    }
    else
    {
        screenWidth = 2.f;
        screenHeight = 2 * mRatio;
    }
#ifdef DEBUG
    printf("screenWidth=%f, screenHeight=%f\n", screenWidth, screenHeight);
#endif
    //float height = 2 * tan(mAngle * PI / 360.0);
    //float width = height * mRatio;

    float left = - screenWidth / 2.0;
    float top  = - screenHeight / 2.0;

    float u = x_ndc * screenWidth + left;
    float v = y_ndc * screenHeight + top;
#ifdef DEBUG
    printf("u=%f, v=%f\n", u, v);
#endif
    float near = screenHeight / (2.f * tanf(mAngle / 2.0));
#ifdef DEBUG
    printf("near=%f\n", near);
#endif
    Vec3f originalDir = near * mDirection + u * mHorizontal + v * mUp;

    if (originalDir.Length() != 0)
    {
        originalDir.Normalize();
    }

    Ray r(mCenter, originalDir);
#ifdef DEBUG
    cout<<r<<endl;
#endif
    return r;
}
