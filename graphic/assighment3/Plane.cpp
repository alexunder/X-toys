/*
 * The Plane class derived from Object3D.
 * Implemented by Mohism Research
 */

#include "Plane.h"
#include "hit.h"
#include <GL/gl.h>

Plane::Plane(Vec3f &normal, float d, Material *m)
    : Object3D(m), mNormal(normal), mDistance(d)
{
    ComputeGLPoints();
}

Plane::Plane(Vec3f a, Vec3f b, Vec3f c, Material *m)
    : Object3D(m)
{
    Vec3f ba = b - a;
    Vec3f ca = c - a;

    Vec3f::Cross3(mNormal, ba, ca);
    mDistance = mNormal.Dot3(a);
}

void Plane::ComputeGLPoints()
{
    mNormal.Normalize();
    Vec3f v(1.0, 0.0, 0.0);

    if (v == mNormal)
    {
        v.Set(0.0, 1.0, 0.0);
    }
   
    //Firstly, compute the projected point on the plane from the origin point.
    // (o + t.Normal)*Normal = d, due to the Plane equation.
    Vec3f realOrigin(0.0, 0.0, 0.0);
    Vec3f origin = realOrigin + mDistance*mNormal;

    Vec3f b1;
    Vec3f b2;

    Vec3f::Cross3(b1, v, mNormal);
    Vec3f::Cross3(b2, mNormal, b1);

    /*
     *   p0---------------p1
     *   |                |
     *   |                |
     *   |                |
     *   |                |
     *   |                |
     *   |                | 
     *   p2---------------p3
     */

    p0 = origin - mBigHalfWidth * b1 - mBigHalfWidth * b2;  
    p1 = origin - mBigHalfWidth * b1 + mBigHalfWidth * b2;  
    p2 = origin + mBigHalfWidth * b1 - mBigHalfWidth * b2;  
    p3 = origin + mBigHalfWidth * b1 + mBigHalfWidth * b2;  
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

void Plane::paint(void)
{
    glBegin(GL_QUADS);
    glNormal3f(mNormal.x(), mNormal.y(), mNormal.z());
    glVertex3f(p0.x(), p0.y(), p0.z());
    glVertex3f(p1.x(), p1.y(), p1.z());
    glVertex3f(p3.x(), p3.y(), p3.z());
    glVertex3f(p2.x(), p2.y(), p2.z());
    glEnd();
}
