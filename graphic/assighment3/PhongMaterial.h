/*
 * The class about PhongMaterial.
 * Implemented by Mohism Research
 */

#ifndef __H_PHONGMATERIAL
#define __H_PHONGMATERIAL

#include "material.h"
#include "ray.h"
#include "vectors.h"

class PhongMaterial : public Material
{
public:
    PhongMaterial(const Vec3f &diffuseColor, const Vec3f &specularColor, float exponent);
    Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const;
    void glSetMaterial(void) const;
private:
    Vec3f mHighLightColor;
    float mPhongComponent;
};

#endif
