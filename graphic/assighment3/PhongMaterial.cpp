/*
 * The class about PhongMaterial.
 * Implemented by Mohism Research
 */

#include "PhongMaterial.h"
#include <math.h>

PhongMaterial::PhongMaterial(const Vec3f &diffuseColor, const Vec3f &specularColor, float exponent)
    : Material(diffuseColor), mHighLightColor(specularColor), mPhongComponent(exponent)
{
#ifdef DEBUG
    cout << mDiffuseColor << endl;
    cout << mHighLightColor << endl;
    cout << mPhongComponent << endl;
#endif
}
    
Vec3f PhongMaterial::Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor)
{
    Vec3f eyeDir = ray.getDirection();
    eyeDir.Negative();

    Vec3f eyePlusLight = eyeDir + dirToLight;
    Vec3f h = eyePlusLight.Normalize();

    float hn = h.Dot3(hit.getNormal());
    hn = pow(hn, mPhongComponent);

    Vec3f color = lightColor * mHighLightColor;
    color = hn * color;
    
    return color;
}
