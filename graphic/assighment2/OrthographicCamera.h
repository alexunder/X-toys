/*
 * The OrthographicCamera  class derived from Camera
 * Implemented by Mohism Research
 */

#ifndef __H_ORTHOGRAPHICCAMERA
#define __H_ORTHOGRAPHICCAMERA

#include "Camera.h"
#include "vectors.h"

class OrthographicCamera : public Camera
{
public:
    OrthographicCamera(const Vec3f &center, Vec3f &direction, Vec3f &up, int size);

	Ray generateRay(Vec2f point);
    float getTMin() const;

    CameraType getCameraType()
    {
        return CameraType::Orthographic;
    }
private:
    Vec3f mCenter;
    Vec3f mDirection;
    Vec3f mUp;
    Vec3f mHorizontal;
    int mCameraSize;
};

#endif
