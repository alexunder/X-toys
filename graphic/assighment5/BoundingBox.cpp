#include "BoundingBox.h"
#include <GL/gl.h>
#include <GL/glu.h>

// ====================================================================
// ====================================================================

void BoundingBox::paint() const {
    // draw a wireframe box to represent the boundingbox

    glColor3f(1,1,1);
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);

    glVertex3f(min.x(),min.y(),min.z());
    glVertex3f(max.x(),min.y(),min.z());
    glVertex3f(min.x(),min.y(),min.z());
    glVertex3f(min.x(),max.y(),min.z());
    glVertex3f(max.x(),max.y(),min.z());
    glVertex3f(max.x(),min.y(),min.z());
    glVertex3f(max.x(),max.y(),min.z());
    glVertex3f(min.x(),max.y(),min.z());

    glVertex3f(min.x(),min.y(),min.z());
    glVertex3f(min.x(),min.y(),max.z());
    glVertex3f(min.x(),max.y(),min.z());
    glVertex3f(min.x(),max.y(),max.z());
    glVertex3f(max.x(),min.y(),min.z());
    glVertex3f(max.x(),min.y(),max.z());
    glVertex3f(max.x(),max.y(),min.z());
    glVertex3f(max.x(),max.y(),max.z());

    glVertex3f(min.x(),min.y(),max.z());
    glVertex3f(max.x(),min.y(),max.z());
    glVertex3f(min.x(),min.y(),max.z());
    glVertex3f(min.x(),max.y(),max.z());
    glVertex3f(max.x(),max.y(),max.z());
    glVertex3f(max.x(),min.y(),max.z());
    glVertex3f(max.x(),max.y(),max.z());
    glVertex3f(min.x(),max.y(),max.z());

    glEnd();
    glEnable(GL_LIGHTING);       
}

BoundingBox Union(const BoundingBox &b, const Vec3f &p)
{
    BoundingBox ret = b;

    Vec3f minP( min(b.getMin().x(), p.x()),
                min(b.getMin().y(), p.y()),
                min(b.getMin().z(), p.z()) );

    Vec3f maxP( max(b.getMax().x(), p.x()),
                max(b.getMax().y(), p.y()),
                max(b.getMax().z(), p.z()) );

    ret.Set(minP, maxP);
}

BoundingBox Union(const BoundingBox &b, const BoundingBox &b2)
{
    BoundingBox ret;

    Vec3f minP( min(b.getMin().x(), b2.getMin().x()),
                min(b.getMin().y(), b2.getMin().y()),
                min(b.getMin().z(), b2.getMin().z()) );

    Vec3f maxP( max(b.getMax().x(), b2.getMax().x()),
                max(b.getMax().y(), b2.getMax().y()),
                max(b.getMax().z(), b2.getMax().x()) );

    ret.Set(minP, maxP);
}

bool BoundingBox::IntersectP(const Ray &ray, float *hitt0,float *hitt1) const
{
    float t0 = Ray::mint;
    float t1 = Ray::maxt;
    for (int i = 0; i < 3; ++i)
    {
        Vec3f dir = ray.getDirection();
        Vec3f org = ray.getOrigin();
        // Update interval for _i_th bounding box slab
        float invRayDir = 1.f / dir[i];
        float tNear = (min[i] - org[i]) * invRayDir;
        float tFar  = (max[i] - org[i]) * invRayDir;
        // Update parametric interval from slab intersection $t$s
        if (tNear > tFar)
        {
           // swap(tNear, tFar);
           float temp = tNear;
           tNear = tFar;
           tFar = temp;
        }

        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar  < t1 ? tFar  : t1;
        if (t0 > t1) return false;
    }
    if (hitt0) *hitt0 = t0;
    if (hitt1) *hitt1 = t1;
    return true;
}
// ====================================================================
// ====================================================================
