/*
 * The Plane class derived from Object3D.
 * Implemented by Mohism Research
 */

#include "Plane.h"
#include "hit.h"

Plane::Plane(Vec3f &normal, float d, Material *m)
    : Object3D(m), mNormal(normal), mDistance(d)
{
    
}

Plane::Plane(Vec3f a, Vec3f b, Vec3f c, Material *m)
    : Object3D(m)
{
    Vec3f ba = b - a;
    Vec3f ca = c - a;

    Vec3f::Cross3(mNormal, ba, ca);
    mDistance = mNormal.Dot3(a);
}

bool Plane::intersect(const Ray &r, Hit &h, float tmin)
{
    Vec3f original = r.getOrigin();
    Vec3f dir = r.getDirection();

    //(original +  t * dir)*Normal = distance
    //a + t * b = d
    float a = original.Dot3(mNormal);
    float b = dir.Dot3(mNormal);

    double t = (mDistance - a) / b;

    if (t < tmin)
        return false;

    h.set(t, mMaterial, mNormal, r);
    return true;
}
