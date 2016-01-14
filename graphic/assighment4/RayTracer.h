/*
 * The RayTracer class
 * Implemented by Mohism Research
 */

#ifndef __H_RAYTRACER
#define __H_RAYTRACER

class RayTracer
{
public:
    RayTracer(SceneParser *s, int max_bounces, float cutoff_weight, 
              bool shadows, int width, int height);
    Vec3f traceRay(Ray &ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit &hit) const;
private:
    SceneParser * mParser;
    int mBounces;
    int mCutoffWeight;
    bool mRenderShadow;
    int mImageWidth;
    int mImageHeight;
}

#endif
