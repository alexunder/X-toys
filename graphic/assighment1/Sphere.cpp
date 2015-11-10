/*
 * The Sphere class derived from Object3D.
 * Implemented by Mohism Research
 */

#include "Sphere.h"
#include "hit.h"

Sphere::Sphere(const Vec3f &point, float radius, Material *m)
    : mCenterPoint(point), mRadius(radius)
{

}

bool Sphere::intersect(const Ray &r, Hit &h, float tmin)
{
    return true;
}
