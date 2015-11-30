/*
 * The class about Camera.
 * Implemented by Mohism Research
 */

#ifndef __H_CAMERA
#define __H_CAMERA

#include "vectors.h"
#include "ray.h"

enum class CameraType {Base, Orthographic, Perspective};

class Camera
{
public:
    Camera() {};
	virtual Ray generateRay(Vec2f point) = 0;
	virtual float getTMin() const = 0;
    virtual CameraType getCameraType()
    {
        return CameraType::Base;
    }
};

#endif
