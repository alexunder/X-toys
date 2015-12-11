/*
 * The Triangle class derived from Object3D.
 * Implemented by Mohism Research
 */

#ifndef __H_TRIANGLE
#define __H_TRIANGLE

#include "Object3D.h"
#include "vectors.h"
#include "material.h"

class Triangle : public Object3D
{
public:
    Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m);
    bool intersect(const Ray &r, Hit &h, float tmin);
private:
    Vec3f mP0;
    Vec3f mP1;
    Vec3f mP2;
};

#endif