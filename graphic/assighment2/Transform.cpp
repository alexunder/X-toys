/*
 * The Transform class derived from Object3D.
 * Implemented by Mohism Research
 */

#include "Transform.h"
#include "hit.h"

Transform::Transform(Matrix &m, Object3D *o)
	:mMat(m), mObj(o)
{
	mMat.Inverse(mReverseMat); 
}
    
bool Transform::intersect(const Ray &r, Hit &h, float tmin)
{
	Vec3f original = r.getOrigin();
	Vec3f dir = r.getDirection();

	mReverseMat.Transform(original);
	mReverseMat.TransformDirection(dir);


	Ray transformedRay(original, dir);

	if (mObj->intersect(transformedRay, h, tmin))
	{
		
	}
}
