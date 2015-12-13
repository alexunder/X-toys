/*
 * The Transform class derived from Object3D.
 * Implemented by Mohism Research
 */

#ifndef __H_TRANSFORM
#define __H_TRANSFORM

#include "Object3D.h"
#include "matrix.h"

class Transform : public Object3D
{
public:
	Transform(Matrix &m, Object3D *o);
    bool intersect(const Ray &r, Hit &h, float tmin);
private:
	Object3D * mObj;
	Matrix     mMat;
	Matrix     mReverseMat;
};

#endif
