/*
 * The RayTracer class
 * Implemented by Mohism Research
 */

#ifndef __H_RAYTRACER
#define __H_RAYTRACER

#include "vectors.h"
#include "scene_parser.h"
#include "hit.h"
#include "ray.h"

class RayTracer
{
public:
    RayTracer(SceneParser *s, int max_bounces, float cutoff_weight, 
              bool shadows, bool shadeback);
    Vec3f traceRay(const Ray &ray, float tmin, int bounces, float weight, 
                           float indexOfRefraction, Hit &hit);
private:
    Vec3f mirrorDirection(const Vec3f &normal, const Vec3f &incoming) const;
    bool transmittedDirection(const Vec3f &normal, const Vec3f &incoming,
            float index_i, float index_t, Vec3f &transmitted) const;
private:
    SceneParser * mParser;
    int mBounces;
    int mCutoffWeight;
    bool mRenderShadow;
	bool mShadeBack;
};

#endif
