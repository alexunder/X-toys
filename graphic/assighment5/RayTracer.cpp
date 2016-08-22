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
#include "trace.h"

#include <math.h>

#define LOG_TAG "RayTracer"

const float epsilon = 0.0000001;

RayTracer::RayTracer(SceneParser *s, int max_bounces, float cutoff_weight,
    bool shadows, bool shadeback, Grid * grid, Matrix * matrix)
    : mParser(s), mBounces(max_bounces), mCutoffWeight(cutoff_weight), 
      mRenderShadow(shadows), mShadeBack(shadeback), mGrid(grid)
{
    mParser->getGroup()->insertIntoGrid(mGrid, matrix);
}
/*
Vec3f RayTracer::traceRayPro(const Ray &ray, float tmin, int bounces, float weight,
                            float indexOfRefrection, Hit &hit) {
	Camera * pCamera = mParser->getCamera();
    float tm = pCamera->getTMin();
    Vec3f backColor = mParser->getBackgroundColor();
	Vec3f ambientLight = mParser->getAmbientLight();

    Group * objGroups = mParser->getGroup();
    int numberLights = mParser->getNumLights();

    Vec3f pixelColor(0.0, 0.0, 0.0);

    bool ishit = false;
    ishit = grid->intersect(ray, hit, tm);

    if (ishit) {
         
        if (bounces == 0)
            RayTree::SetMainSegment(ray, 0, hit.getT());

        Material * pM = hit.getMaterial();
        Vec3f normal = hit.getNormal();
        Vec3f point = hit.getIntersectionPoint();
        Vec3f diffuseColor = pM->getDiffuseColor();
        pixelColor = diffuseColor * ambientLight; 
    }
}
*/
Vec3f RayTracer::traceRay(const Ray &ray, float tmin, int bounces, float weight, 
                                    float indexOfRefraction, Hit &hit)
{
    log_trace(LOG_TAG, "enter traceRay, weight=%f", weight);
	Camera * pCamera = mParser->getCamera();
    float tm = pCamera->getTMin();
    Vec3f backColor = mParser->getBackgroundColor();
	Vec3f ambientLight = mParser->getAmbientLight();

    Group * objGroups = mParser->getGroup();
    int numberLights = mParser->getNumLights();

    Vec3f pixelColor(0.0, 0.0, 0.0);

    bool ishit = false;
#ifdef NON_OPT
    ishit = objGroups->intersect(ray, hit, tm);
#else
    ishit = mGrid->intersect(ray, hit, tm);
#endif

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
        cout << "diffuse&ambient pixel color:" << pixelColor << endl;

        int k;
        for (k = 0; k < numberLights; k++)
        {
            Light * plight = mParser->getLight(k);

            Vec3f lightDir;
            Vec3f lightColor;
            float distance;
            plight->getIllumination(point, lightDir, lightColor, distance);
            float d = lightDir.Dot3(normal);

            if (d < epsilon)
            {
                d = -d;
            }

            Vec3f tempColor = lightColor * diffuseColor;
            tempColor *= d;
            tempColor += pM->Shade(ray, hit, lightDir, lightColor);
            cout << "tempColor=" << tempColor << endl;

            if (mRenderShadow)
            {
                Ray shadowRay(point, lightDir);

				Hit shadowHit;
                //Be careful, tm should be larger than one
                if (!objGroups->intersect(shadowRay, shadowHit, tm))
                {
                    pixelColor += tempColor;
					RayTree::AddShadowSegment(shadowRay, 0, 100000.0);
                }
				else
				{
					RayTree::AddShadowSegment(shadowRay, 0, shadowHit.getT());
				}
            }
            else
            {
                pixelColor += tempColor;
                
            }
            cout << "Shadow pixel Color=" << pixelColor << endl;
        }

        Vec3f rc = pM->getReflectiveColor();
        cout << "Reflective color=" << rc << endl;
        //Process the reflective situation
        if (rc.Length() > 0.0 && bounces < mBounces)
        {
            printf("handle reflective.\n");
            Vec3f incomingDir = ray.getDirection();
            Vec3f reflectiveDir = mirrorDirection(normal, incomingDir);
            Ray reflectiveRay(point, reflectiveDir);
            Hit reflectiveHit;
            pixelColor += rc*traceRay(reflectiveRay, tm, bounces + 1, weight*rc.Length(),
                                                indexOfRefraction, reflectiveHit);
            cout << "reflective pixel color:" << pixelColor << endl;
            //Reflective ray debug
			printf("reflective T=%.20lf\n", reflectiveHit.getT());
			//float t = reflectiveHit.getT() > 0.0 ? reflectiveHit.getT() : 10000.0;
            float t = reflectiveHit.getT();
            if (t < epsilon)
                t = 10000.0;

            printf("t=%.20lf\n", t);
            RayTree::AddReflectedSegment(reflectiveRay, 0, t);
        }

        Vec3f tc = pM->getTransparentColor();
        //Process the refraction situation
        if (tc.Length() > 0.0 && bounces < mBounces)
        {
            printf("handle refraction.\n");
            Vec3f incomingDir = ray.getDirection();
            float flag = incomingDir.Dot3(normal);

			cout << "normal=" << normal << endl;
			cout << "incomingDir" << incomingDir << endl;
			cout << "flag=" << flag << endl;

            bool success = false; 

            float index_incident = indexOfRefraction;
            float index_material = pM->getIndexOfRefraction();

			printf("index_incident=%f\n", index_incident);
			printf("index_material=%f\n", index_material);
            Vec3f transimittedDir;

            Vec3f transparentColor(0.0, 0.0, 0.0);

            Hit transimittedHit;
			
			if (flag < 0)//outside-->inside
			{
				printf("outside-->inside\n");
				if (transmittedDirection(normal, incomingDir, index_incident, index_material,
							transimittedDir))
				{
					Ray refractionRay(point, transimittedDir);
					transparentColor =  tc*traceRay(refractionRay, tm, bounces + 1, weight*tc.Length(),
							indexOfRefraction, transimittedHit);
                    cout << "Transmitted Color:" << transparentColor << endl;
					RayTree::AddTransmittedSegment(refractionRay, 0, transimittedHit.getT());
				}
			}
			else
			{
				printf("inside-->outside\n");
                Vec3f n(normal);
                n.Negate();
				if (transmittedDirection(n, incomingDir, index_material, index_incident, 
							transimittedDir))
				{
					Ray refractionRay(point, transimittedDir);
					transparentColor = tc*traceRay(refractionRay, tm, bounces + 1, weight*tc.Length(),
							indexOfRefraction, transimittedHit);

                    cout << "Transmitted Color:" << transparentColor << endl;
					RayTree::AddTransmittedSegment(refractionRay, 0, transimittedHit.getT());
				}
			}
            pixelColor += transparentColor; 
            cout << "Final pixel Color=" << pixelColor << endl;
        }
        pixelColor.Clamp();
        log_trace(LOG_TAG, "exit traceRay");
        return pixelColor;
    }
    else
    {
        log_trace(LOG_TAG, "exit traceRay");
        return backColor;
    }
}
    
Vec3f RayTracer::mirrorDirection(const Vec3f &normal, const Vec3f &incoming) const
{
	Vec3f N(normal);
	Vec3f L(incoming);
	N.Normalize();
	L.Normalize();
    float d = N.Dot3(L);
    Vec3f reflection = (L - 2*d*N);
    return reflection; 
}

bool RayTracer::transmittedDirection(const Vec3f &normal, const Vec3f &incoming,
       float index_i, float index_t, Vec3f &transmitted) const
{
    printf("entry transmittedDirection.\n");
    Vec3f N(normal);
    N.Normalize();
    Vec3f L(incoming);
    L.Negate();
    L.Normalize();
    float cosin_incident = N.Dot3(L);
    float sin_incident_sqr = 1 - cosin_incident*cosin_incident; 
    float ratio = index_i / index_t;
	float temp = 1 - ratio*ratio*sin_incident_sqr;

	if (temp >= 0)
	{
		printf("Transmitted temp is positive.\n");
    	float weight = ratio*cosin_incident - sqrt(1 - ratio*ratio*sin_incident_sqr);
    	transmitted = weight*N - ratio*L;
	}
	else
	{
		printf("Transmitted temp is negative.\n");
		N.Negate();
		transmitted = mirrorDirection(N, incoming);
	}
    cout << "Transmitted is " << transmitted <<endl;
	return true;
}
