/*
 * The Transform class derived from Object3D.
 * Implemented by Mohism Research
 */

#include "Transform.h"
#include "hit.h"
#include "BoundingBox.h"
#include <GL/gl.h>

Transform::Transform(Matrix &m, Object3D *o)
	:mMat(m), mObj(o)
{
	mMat.Inverse(mReverseMat);

    //Compute the transformed BBox
    BoundingBox *pBB = mObj->getBoundingBox();

    if (pBB == NULL)
        return;

    Vec3f minP = pBB->getMin();
    Vec3f maxP = pBB->getMax();

    mMat.Transform(minP);
    mMat.Transform(maxP);

    BoundingBox box;
    box = Union(box, minP);
    box = Union(box, Vec3f(maxP.x(), minP.y(), minP.z()));
    box = Union(box, Vec3f(minP.x(), maxP.y(), minP.z()));
    box = Union(box, Vec3f(maxP.x(), maxP.y(), minP.z()));
    box = Union(box, Vec3f(minP.x(), minP.y(), maxP.z()));
    box = Union(box, Vec3f(maxP.x(), minP.y(), maxP.z()));
    box = Union(box, Vec3f(minP.x(), maxP.y(), maxP.z()));
    box = Union(box, maxP);

    mpBox = new BoundingBox();
    mpBox->Set(&box);
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
        Vec3f normal = h.getNormal();
        Matrix t;
        mReverseMat.Transpose(t);
        t.TransformDirection(normal);
        h.set(h.getT(), h.getMaterial(), normal, r);
        return true;
	}

    return false;
}
void Transform::paint(void)
{
	glPushMatrix();
	GLfloat *glMatrix = mMat.glGet();
	glMultMatrixf(glMatrix);
	delete[] glMatrix;

	mObj->paint();
	glPopMatrix();
}
