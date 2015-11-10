/*
 * The base class of Object3D.
 * Implemented by Mohism Research
 */

#ifndef __H_OBJECT3D
#define __H_OBJECT3D

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

    virtual ~Object3D()
    {
    }

    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;
private:
    Material * mMaterial;
}

#endif
