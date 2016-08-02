/*
 * The Sphere class derived from Object3D.
 * Implemented by Mohism Research
 */

#ifndef __H_SPHERE
#define __H_SPHERE

#include "Object3D.h"
#include "vectors.h"
#include "material.h"

class Sphere : public Object3D
{
public:
    Sphere(const Vec3f &point, float radius, Material *m);
    bool intersect(const Ray &r, Hit &h, float tmin);
    void paint(void);
    void insertIntoGrid(Grid *g, Matrix *m);
    static void setTesselationSize(int theta, int phi);
private:
    float mRadius;
    Vec3f mCenterPoint;
    static int mThetaSteps;
    static int mPhiSteps;
};

#endif
