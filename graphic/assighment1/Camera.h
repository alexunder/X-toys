/*
 * The class about Camera.
 * Implemented by Mohism Research
 */

#ifndef __H_CAMERA
#define __H_CAMERA

class Vec2f;
class Ray;

class Camera
{
public:
	virtual Ray generateRay(Vec2f point) = 0;
	virtual float getTMin() const = 0;
};

#endif
