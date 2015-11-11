/*
 * The Sphere class derived from Object3D.
 * Implemented by Mohism Research
 */

#include "Sphere.h"
#include "hit.h"

Sphere::Sphere(const Vec3f &point, float radius, Material *m)
    : mCenterPoint(point), mRadius(radius)
{
    mMaterial = m;
}

bool Sphere::intersect(const Ray &r, Hit &h, float tmin)
{
    Vec3f temp = r.getOrigin() - mCenterPoint;
    Vec3f rayDirection = r.getDirection();

    double a = rayDirection.Dot3(rayDirection);
    double b = 2*rayDirection.Dot3(temp);
    double c = temp.Dot3(temp) - mRadius*mRadius;

#ifdef DEBUG
    cout << "temp="  << temp<<endl;
    printf("Sphere::intersect, a=%f, b=%f, c=%f\n", a, b, c);
#endif

    double discriminant = b*b - 4*a*c;

    if (discriminant > 0)
    {
        discriminant = sqrt(discriminant);
        double t = (- b - discriminant) / (2*a);

        if (t < tmin)
            t = (- b + discriminant) / (2*a);

        if (t < tmin || t > tmax)
            return false;

#ifdef DEBUG
        printf("Sphere::intersect, there is a hit, t=%f\n", t);
#endif
        h.set(t, mMaterial, r);
        return true;
    }

    return false;
}
