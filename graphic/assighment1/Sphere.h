/*
 * The Sphere class derived from Object3D.
 * Implemented by Mohism Research
 */

#ifndef __H_SPHERE
#define __H_SPHERE

#include "Object3D.h"
#include "vector.h"
#include "material.h"

 class Sphere : public Object3D
 {
public:
    Sphere(const Vec3f &point, float radius, Material *m);
    bool intersect(const Ray &r, Hit &h, float tmin);
private:
    float mRadius;
    Vec3f mCenterPoint;
 };

#endif
