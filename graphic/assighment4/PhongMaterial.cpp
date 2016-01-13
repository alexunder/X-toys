/*
 * The class about PhongMaterial.
 * Implemented by Mohism Research
 */

#include "PhongMaterial.h"
#include "glCanvas.h"
#include <math.h>
#include <GL/gl.h>

#ifdef SPECULAR_FIX
// OPTIONAL:  global variable allows (hacky) communication
// with glCanvas::display
extern int SPECULAR_FIX_WHICH_PASS;
#endif

PhongMaterial::PhongMaterial(const Vec3f &diffuseColor, const Vec3f &specularColor, float exponent,
                             const Vec3f &reflectiveColor, const Vec3f &transparentColor,
                             float indexOfRefraction)
    : Material(diffuseColor), mHighLightColor(specularColor), mPhongComponent(exponent), 
      mReflectiveColor(transparentColor), mTransparentColor(transparentColor), mIndexOfRefraction(indexOfRefraction)
{
#ifdef DEBUG
    cout << diffuseColor << endl;
    cout << mHighLightColor << endl;
    cout << mPhongComponent << endl;
#endif
}
    
Vec3f PhongMaterial::Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const
{
    Vec3f eyeDir = ray.getDirection();
    eyeDir.Negate();

    Vec3f eyePlusLight = eyeDir + dirToLight;
    eyePlusLight.Normalize(); 
    
    float hn = eyePlusLight.Dot3(hit.getNormal());
    hn = pow(hn, mPhongComponent);

    Vec3f color = lightColor * mHighLightColor;
    color = hn * color;

    return color;
}


void PhongMaterial::glSetMaterial(void) const
{
    GLfloat one[4] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat zero[4] = { 0.0, 0.0, 0.0, 0.0 };
    GLfloat specular[4] = {
        mHighLightColor.r(),
        mHighLightColor.g(),
        mHighLightColor.b(),
        1.0};
    GLfloat diffuse[4] = {
        diffuseColor.r(),
        diffuseColor.g(),
        diffuseColor.b(),
        1.0};

    // NOTE: GL uses the Blinn Torrance version of Phong...
    float glexponent = mPhongComponent;
    if (glexponent < 0) glexponent = 0;
    if (glexponent > 128) glexponent = 128;

#if !SPECULAR_FIX

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);

#else

    // OPTIONAL: 3 pass rendering to fix the specular highlight
    // artifact for small specular exponents (wide specular lobe)

    if (SPECULAR_FIX_WHICH_PASS == 0) {
        // First pass, draw only the specular highlights
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, zero);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);

    } else if (SPECULAR_FIX_WHICH_PASS == 1) {
        // Second pass, compute normal dot light
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, one);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
    } else {
        // Third pass, add ambient & diffuse terms
        assert (SPECULAR_FIX_WHICH_PASS == 2);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
    }

#endif
}
