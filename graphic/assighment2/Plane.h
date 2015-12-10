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
private:
};

#endif
