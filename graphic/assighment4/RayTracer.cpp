/*
 * The RayTracer class
 * Implemented by Mohism Research
 */

#include "RayTracer.h"


RayTracer::RayTracer(SceneParser *s, int max_bounces, float cutoff_weight, 
                     bool shadows, int width, int height)
    : mParser(s), mBounces(max_bounces), mCutoffWeight(cutoff_weight), 
      mRenderShadow(shadows), mImageWidth(width), mImageHeight(height)
{

}

Vec3f RayTracer::traceRay(Ray &ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit &hit) const
{
	Camera * pCamera = mParser->getCamera();
    float tmin = pCamera->getTMin();
    Vec3f backColor = mParser->getBackgroundColor();
	Vec3f ambientLight = mParser->getAmbientLight();

    Group * objGroups = mParser->getGroup();
    int numberLights = parser->getNumLights();
    
    Vec3f pixelColor(0.0, 0.0, 0.0);

    bool ishit = false;
    Hit h;
    ishit = objGroups->intersect(ray, h, tmin);

    if (ishit)
    {
        Material * pM = h.getMaterial();
        Vec3f normal = h.getNormal();
        Vec3f point = h.getIntersectionPoint();

        Vec3f diffuseColor = pM->getDiffuseColor();
        pixelColor = diffuseColor * ambientLight; 
        
        int k;
        for (k = 0; k < numberLights; k++)
        {
            Light * plight = parser->getLight(k);
                
            Vec3f lightDir;
            Vec3f lightColor;
            float distance;
            plight->getIllumination(point, lightDir, lightColor, distance);
            float d = lightDir.Dot3(normal);
                
            if (d < 0)
                d = 0.0;
           
            Vec3f tempColor = lightColor * diffuseColor;
            tempColor *= d;
            tempColor +=  pM->Shade(ray, h, lightDir, lightColor);

            if (mRenderShadow)
            {
                Ray shadowRay(point, lightDir);
                Hit temphit;
                if (!objGroups->intersect(shadowRay, temphit, tmin))
                {
                    pixelColor += tempColor;
                }
            }
            else
            {
                pixelColor += tempColor;
                
            }
        }

        return pixelColor;
    }
    else
    {
        return backColor;
    }
}
