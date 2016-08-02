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
class Grid;
class Matrix;

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
        mpBox = NULL;
    }

    virtual ~Object3D()
    {
        //if (mpBox != NULL)
        //    delete mpBox;
    }

    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;

    virtual void paint(void) = 0;

    virtual BoundingBox * getBoundingBox()
    {
        return mpBox;
    }

    virtual void insertIntoGrid(Grid *g, Matrix *m)
    {
        return;
    }
protected:
    Material * mMaterial;
    BoundingBox * mpBox;
};
#endif
