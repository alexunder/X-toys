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
    Plane(Vec3f a, Vec3f b, Vec3f c, Material *m00);
    bool intersect(const Ray &r, Hit &h, float tmin);
private:
    Vec3f mNormal;
    float mDistance;
};

#endif
