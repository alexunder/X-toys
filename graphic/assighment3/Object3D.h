/*
 * The base class of Object3D.
 * Implemented by Mohism Research
 */

#ifndef __H_OBJECT3D
#define __H_OBJECT3D

#include <cstddef>
#include <GL/gl.h>

class Material;
class Ray;
class Hit;

class Object3D
{
public:
    Object3D(Material * m)
        :mMaterial(m)
    {
    }

    Object3D()
    {
        mMaterial = NULL;
    }

    virtual ~Object3D()
    {
    }

    Material * getMaterial()
    {
        return mMaterial;
    }

    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;

    virtual void paint(void) = 0;
protected:
    Material * mMaterial;
};

#endif
