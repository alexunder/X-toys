/*
 * The implementation of the OrthographicCamera class
 * Implemented by Mohism Research
 */

#include "OrthographicCamera.h"

OrthographicCamera::OrthographicCamera(const Vec3f &center, Vec3f &direction, Vec3f &up, int size)
    : mCenter(center), mDirection(direction), mUp(up), mCameraSize(size)
{
    if (mDirection.Length() != 1)
    {
        mDirection.Normalize();    
    }

    if (mUp.Length() != 1)
    {
        mUp.Normalize();    
    }    
}

Ray OrthographicCamera::generateRay(Vec2f point)
{

}
    
float OrthographicCamera::getTMin() const
{

}