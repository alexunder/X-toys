/*
 * The Sphere class derived from Object3D.
 * Implemented by Mohism Research
 */

#include "Sphere.h"
#include "hit.h"
#include "BoundingBox.h"
#include "Grid.h"

#include <math.h>
#include <GL/gl.h>

#define T_MAX 100000.0f
#define PI    3.1415926

int Sphere::mThetaSteps = 5;
int Sphere::mPhiSteps = 10;

Sphere::Sphere(const Vec3f &point, float radius, Material *m)
    : Object3D(m), mCenterPoint(point), mRadius(radius)
{
#ifdef DEBUG
    printf("Sphere, mCenterPoint = (x=%f,y=%f,z=%f)\n", mCenterPoint[0], mCenterPoint[1], mCenterPoint[2]);
    printf("radius=%f\n", mRadius);
#endif
    //Computing the bounfding Box
    Vec3f minPoint(mCenterPoint.x() - mRadius, 
                   mCenterPoint.y() - mRadius,
                   mCenterPoint.z() - mRadius);
    
    Vec3f maxPoint(mCenterPoint.x() + mRadius, 
                   mCenterPoint.y() + mRadius,
                   mCenterPoint.z() + mRadius);
    
    mpBox = new BoundingBox(minPoint, maxPoint);
}

bool Sphere::intersect(const Ray &r, Hit &h, float tmin)
{
    Vec3f temp = r.getOrigin() - mCenterPoint;
    Vec3f rayDirection = r.getDirection();

    double a = rayDirection.Dot3(rayDirection);
    double b = 2*rayDirection.Dot3(temp);
    double c = temp.Dot3(temp) - mRadius*mRadius;

#ifdef DEBUG
    //cout << "temp="  << temp<<endl;
    //printf("Sphere::intersect, a=%f, b=%f, c=%f\n", a, b, c);
#endif

    double discriminant = b*b - 4*a*c;

    if (discriminant >= 0)
    {
        discriminant = sqrt(discriminant);
        double t = (- b - discriminant) / (2*a);

        if (t < tmin)
            t = (- b + discriminant) / (2*a);

        if (t < tmin || t > T_MAX)
            return false;

#ifdef DEBUG
      //  printf("Sphere::intersect, there is a hit, t=%f\n", t);
#endif
        Vec3f normal = r.getOrigin() + t * r.getDirection() - mCenterPoint;
        normal.Normalize();
        h.set(t, mMaterial, normal, r);
        return true;
    }

    return false;
}

void Sphere::paint(void)
{
    float thetaRange = 2*PI;
    float phiRange   = PI;
    float thetaStart = 0.0;
    float phiStart = -PI/2.0;
    float thetaDelta = thetaRange / mThetaSteps;
    float phiDelta = phiRange / mPhiSteps;

    mMaterial->glSetMaterial();

    int i;
    int j;
    glBegin(GL_QUADS);
    for (i = 0; i < mPhiSteps; i++)
    for (j = 0; j < mThetaSteps; j++)
    {
        // compute appropriate coordinates & normals
        float curPhi = i * phiDelta;
        float curTheta = j * thetaDelta;
        float nextPhi = (i + 1) * phiDelta;
        float nextTheta = (j + 1) * thetaDelta;

        Vec3f p0(mRadius * sin(curTheta) * cos(curPhi),
                 mRadius * sin(curTheta) * sin(curPhi),
                 mRadius * cos(curTheta));

        p0 += mCenterPoint;
        Vec3f n0 = p0 - mCenterPoint;
        n0.Normalize();

        Vec3f p1(mRadius * sin(curTheta) * cos(nextPhi),
                 mRadius * sin(curTheta) * sin(nextPhi),
                 mRadius * cos(curTheta));

        p1 += mCenterPoint;
        Vec3f n1 = p1 - mCenterPoint;
        n1.Normalize();

        Vec3f p2(mRadius * sin(nextTheta) * cos(curPhi),
                 mRadius * sin(nextTheta) * sin(curPhi),
                 mRadius * cos(nextTheta));

        p2 += mCenterPoint;
        Vec3f n2 = p2 - mCenterPoint;
        n2.Normalize();

        Vec3f p3(mRadius * sin(nextTheta) * cos(nextPhi),
                 mRadius * sin(nextTheta) * sin(nextPhi),
                 mRadius * cos(nextTheta));

        p3 += mCenterPoint;
        Vec3f n3 = p3 - mCenterPoint;
        n3.Normalize();

        //Actually, what I have done is just the Gourand shading, four normals for
        //the polygon
        glNormal3f(n0.x(), n0.y(), n0.z());
        glVertex3f(p0.x(), p0.y(), p0.z());

        glNormal3f(n1.x(), n1.y(), n1.z());
        glVertex3f(p1.x(), p1.y(), p1.z());
        
        glNormal3f(n3.x(), n3.y(), n3.z());
        glVertex3f(p3.x(), p3.y(), p3.z());
        
        glNormal3f(n2.x(), n2.y(), n2.z());
        glVertex3f(p2.x(), p2.y(), p2.z());
    }
    glEnd();
}

void Sphere::setTesselationSize(int theta, int phi)
{
    mThetaSteps = theta;
    mPhiSteps = phi;
}

void Sphere::insertIntoGrid(Grid *g, Matrix *m)
{
#ifdef DEBUG
    printf("Sphere::insertIntoGrid.\n");
#endif
    int i;
    int j;
    int k;

    if (mpBox == NULL)
        return;

    Vec3f minp = mpBox->getMin();
    Vec3f maxp = mpBox->getMax();

    float xBox = maxp.x() - minp.x();
    float yBox = maxp.y() - minp.y();
    float zBox = maxp.z() - minp.z();

    int xSize = g->getXSize();
    int ySize = g->getYSize();
    int zSize = g->getZSize();

    float xDelta = xBox / xSize;
    float yDelta = yBox / ySize;
    float zDelta = zBox / zSize;

    for (k = 0; k < zSize; k++)
    for (j = 0; j < ySize; j++)
    for (i = 0; i < xSize; i++)
    {
        //Get the min point
        Vec3f curPoint(minp.x() + (i + 0.5)*xDelta,
                       minp.y() + (j + 0.5)*yDelta,
                       minp.z() + (k + 0.5)*zDelta );

        //Computing the distance between the voxel and the center point;
        Vec3f temp = curPoint - mCenterPoint;
        float distance = temp.Length();

        if (distance <= mRadius)
            printf("Overlapped in %d %d %d\n", i, j, k);
            g->AddObjectToGrid(this, i, j, k);
    }
}
