/*
 * The Plane class derived from Object3D.
 * Implemented by Mohism Research
 */

#ifndef __H_PLANE
#define __H_PLANE

#include "Object3D.h"
#include "vectors.h"
#include "material.h"

class Plane : public Object3D
{
public:
    Plane(Vec3f &normal, float d, Material *m);
    Plane(Vec3f a, Vec3f b, Vec3f c, Material *m);
    bool intersect(const Ray &r, Hit &h, float tmin);
    void paint(void);
private:
    void ComputeGLPoints();
private:
    Vec3f mNormal;
    float mDistance;

    const float mBigHalfWidth = 20000.0;
    Vec3f p0;
    Vec3f p1;
    Vec3f p2;
    Vec3f p3;
};

#endif
