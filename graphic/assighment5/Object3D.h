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
class BoundingBox;

class Object3D
{
public:
    Object3D(Material * m)
        :mMaterial(m)
    {
        mpBox = NULL;
    }

    Object3D()
    {
        mMaterial = NULL;

        if (mpBox != NULL)
            delete mpBox;
    }

    virtual ~Object3D()
    {
    }

    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;

    virtual void paint(void) = 0;

    virtual BoundingBox * getBoundingBox()
    {
        return mpBox;
    }
protected:
    Material * mMaterial;
    BoundingBox * mpBox;
};

//Some tools' functions



#endif
