/*
 * The RayTracer class
 * Implemented by Mohism Research
 */

#include "RayTracer.h"
#include "material.h"
#include "Camera.h"
#include "scene_parser.h"
#include "Group.h"
#include "light.h"
#include "rayTree.h"

#include <math.h>

RayTracer::RayTracer(SceneParser *s, int max_bounces, float cutoff_weight,
    bool shadows)
    : mParser(s), mBounces(max_bounces), mCutoffWeight(cutoff_weight), 
      mRenderShadow(shadows)
{

}

Vec3f RayTracer::traceRay(Ray &ray, float tmin, int bounces, float weight, 
                                    float indexOfRefraction, Hit &hit) const
{
	Camera * pCamera = mParser->getCamera();
    float tm = pCamera->getTMin();
    Vec3f backColor = mParser->getBackgroundColor();
	Vec3f ambientLight = mParser->getAmbientLight();

    Group * objGroups = mParser->getGroup();
    int numberLights = mParser->getNumLights();

    Vec3f pixelColor(0.0, 0.0, 0.0);

    bool ishit = false;
    ishit = objGroups->intersect(ray, hit, tm);

    if (ishit)
    {
        //The main
        if (bounces == 0)
            RayTree::SetMainSegment(ray, 0, hit.getT());

        Material * pM = hit.getMaterial();
        Vec3f normal = hit.getNormal();
        Vec3f point = hit.getIntersectionPoint();

        Vec3f diffuseColor = pM->getDiffuseColor();
        pixelColor = diffuseColor * ambientLight; 

        int k;
        for (k = 0; k < numberLights; k++)
        {
            Light * plight = mParser->getLight(k);

            Vec3f lightDir;
            Vec3f lightColor;
            float distance;
            plight->getIllumination(point, lightDir, lightColor, distance);
            float d = lightDir.Dot3(normal);

            if (d < 0)
                d = 0.0;

            Vec3f tempColor = lightColor * diffuseColor;
            tempColor *= d;
            tempColor += pM->Shade(ray, hit, lightDir, lightColor);

            if (mRenderShadow)
            {
                Ray shadowRay(point, lightDir);
                Hit shadowHit;
                //Be careful, tm should be larger than one
                if (!objGroups->intersect(shadowRay, shadowHit, tm))
                {
                    pixelColor += tempColor;
                }
                else
                {   //Shadow ray debug
                    RayTree::AddShadowSegment(shadowRay, 0, shadowHit.getT());
                }
            }
            else
            {
                pixelColor += tempColor;
                
            }
        }

        cout << "Reflective color=" << pM->getReflectiveColor() << endl;
        //Process the reflective situation
        if (pM->getReflectiveColor().Length() > 0.0 && bounces < mBounces)
        {
            printf("handle reflective.\n");
            Vec3f incomingDir = ray.getDirection();
            Vec3f reflectiveDir = mirrorDirection(normal, incomingDir);
            Ray reflectiveRay(point, reflectiveDir);
            Hit reflectiveHit;
            pixelColor += weight*traceRay(reflectiveRay, tm, bounces + 1, weight,
                                                indexOfRefraction, reflectiveHit);

            //Reflective ray debug
            RayTree::AddReflectedSegment(reflectiveRay, 0, reflectiveHit.getT());
        }

        //Process the refraction situation
        if (pM->getTransparentColor().Length() > 0.0 && bounces < mBounces)
        {
            printf("handle refraction.\n");
            Vec3f incomingDir = ray.getDirection();
            float flag = incomingDir.Dot3(normal);

            bool success = false; 

            float index_incident = indexOfRefraction;
            float index_material = pM->getIndexOfRefraction();

            Vec3f transimittedDir;

            Vec3f transparentColor(0.0, 0.0, 0.0);

            Hit transimittedHit;

            if (flag > 0.0)
            {
               if (transmittedDirection(normal, incomingDir, index_material, index_incident,
                   transimittedDir))
               {
                   Ray refractionRay(point, transimittedDir);
                   transparentColor =  weight*traceRay(refractionRay, tm, bounces + 1, weight, 
                   index_incident, transimittedHit);

                   RayTree::AddTransmittedSegment(refractionRay, 0, transimittedHit.getT());
               }
            }
            else if (flag < 0.0)
            {
               if (transmittedDirection(normal, incomingDir, index_incident, index_material,
                   transimittedDir))
               {
                   Ray refractionRay(point, transimittedDir);
                   transparentColor =  weight*traceRay(refractionRay, tm, bounces + 1, weight,
                   index_material, transimittedHit);

                   RayTree::AddTransmittedSegment(refractionRay, 0, transimittedHit.getT());
               }

            }

            pixelColor += transparentColor; 
        }

        return pixelColor;
    }
    else
    {
        return backColor;
    }
}
    
Vec3f RayTracer::mirrorDirection(const Vec3f &normal, const Vec3f &incoming) const
{
   float d = normal.Dot3(incoming);
   Vec3f reflection = (incoming - 2*d*normal);
   return reflection; 
}

bool RayTracer::transmittedDirection(const Vec3f &normal, const Vec3f &incoming,
       float index_i, float index_t, Vec3f &transmitted) const
{
    Vec3f N(normal);
    N.Normalize();
    Vec3f L(incoming);
    L.Negate();
    L.Normalize();
    float cosin_incident = N.Dot3(L);
    float sin_incident_sqr = 1 - cosin_incident*cosin_incident; 
    float ratio = index_i / index_t;
    float weight = ratio*cosin_incident - sqrt(1 - ratio*ratio*sin_incident_sqr);
    transmitted = weight*N - ratio*L;
}
