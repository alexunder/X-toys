/*
 * The PerspectiveCamera class derived from Camera
 * Implemented by Mohism Research
 */

#include "PerspectiveCamera.h"

#include <math.h>

#define PI 3.14159265

PerspectiveCamera::PerspectiveCamera(Vec3f &center, Vec3f &direction, Vec3f &up, float angle)
    : mCenter(center), mDirection(direction), mUp(up), mAngle(angle)
{
    float v = mUp.Dot3(mDirection);

    if (v != 0)
    {
        Vec3f temp;
        Vec3f::Cross3(temp, mDirection, mUp);
        Vec3f::Cross3(mUp, temp, mDirection);
    }

    if (mDirection.Length() != 1)
    {
        mDirection.Normalize();    
    }

    if (mUp.Length() != 1)
    {
        mUp.Normalize();    
    }

    Vec3f::Cross3(mHorizontal, mDirection, mUp);

    mRatio = 1.0;
#ifdef DEBUG
    printf("mAngle=%f\n", mAngle);
#endif
}


Ray PerspectiveCamera::generateRay(Vec2f point)
{
    float x_ndc = point.x();
    float y_ndc = point.y();
#ifdef DEBUG
    printf("PerspectiveCamera::generateRay, x_ndc=%f, y_ndc=%f\n", x_ndc, y_ndc);
#endif
    float screenWidth = 0.f;
    float screenHeight = 0.f;

    if (mRatio > 1.f)
    {
        screenWidth = 2 * mRatio;
        screenHeight = 2.f;
    }
    else
    {
        screenWidth = 2.f;
        screenHeight = 2 * mRatio;
    }
#ifdef DEBUG
    printf("screenWidth=%f, screenHeight=%f\n", screenWidth, screenHeight);
#endif
    //float height = 2 * tan(mAngle * PI / 360.0);
    //float width = height * mRatio;

    float left = - screenWidth / 2.0;
    float top  = - screenHeight / 2.0;

    float u = x_ndc * screenWidth + left;
    float v = y_ndc * screenHeight + top;
#ifdef DEBUG
    printf("u=%f, v=%f\n", u, v);
#endif
    float near = screenHeight / (2.f * tanf(mAngle / 2.0));
#ifdef DEBUG
    printf("near=%f\n", near);
#endif
    Vec3f originalDir = near * mDirection + u * mHorizontal + v * mUp;

    if (originalDir.Length() != 0)
    {
        originalDir.Normalize();
    }

    Ray r(mCenter, originalDir);
#ifdef DEBUG
    cout<<r<<endl;
#endif
    return r;
}

// ====================================================================
// Create a perspective camera with the appropriate dimensions that
// crops or stretches in the x-dimension as necessary
// ====================================================================

void PerspectiveCamera::glInit(int w, int h)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(angle*180.0/3.14159, (float)w/float(h), 0.5, 40.0);
}

// ====================================================================
// Place a perspective camera within an OpenGL scene
// ====================================================================

void PerspectiveCamera::glPlaceCamera(void)
{
  gluLookAt(center.x(), center.y(), center.z(),
            center.x()+direction.x(), center.y()+direction.y(), center.z()+direction.z(),
            up.x(), up.y(), up.z());
}

// ====================================================================
// dollyCamera, truckCamera, and RotateCamera
//
// Asumptions:
//  - up is really up (i.e., it hasn't been changed
//    to point to "screen up")
//  - up and direction are normalized
// Special considerations:
//  - If your constructor precomputes any vectors for
//    use in 'generateRay', you will likely to recompute those
//    values at athe end of the these three routines
// ====================================================================

// ====================================================================
// dollyCamera: Move camera along the direction vector
// ====================================================================

void PerspectiveCamera::dollyCamera(float dist)
{
  center += direction*dist;

  // ===========================================
  // ASSIGNMENT 3: Fix any other affected values
  // ===========================================


}

// ====================================================================
// truckCamera: Translate camera perpendicular to the direction vector
// ====================================================================

void PerspectiveCamera::truckCamera(float dx, float dy)
{
  Vec3f horizontal;
  Vec3f::Cross3(horizontal, direction, up);
  horizontal.Normalize();

  Vec3f screenUp;
  Vec3f::Cross3(screenUp, horizontal, direction);

  center += horizontal*dx + screenUp*dy;

  // ===========================================
  // ASSIGNMENT 3: Fix any other affected values
  // ===========================================


}

// ====================================================================
// rotateCamera: Rotate around the up and horizontal vectors
// ====================================================================

void PerspectiveCamera::rotateCamera(float rx, float ry)
{
  Vec3f horizontal;
  Vec3f::Cross3(horizontal, direction, up);
  horizontal.Normalize();

  // Don't let the model flip upside-down (There is a singularity
  // at the poles when 'up' and 'direction' are aligned)
  float tiltAngle = acos(up.Dot3(direction));
  if (tiltAngle-ry > 3.13)
    ry = tiltAngle - 3.13;
  else if (tiltAngle-ry < 0.01)
    ry = tiltAngle - 0.01;

  Matrix rotMat = Matrix::MakeAxisRotation(up, rx);
  rotMat *= Matrix::MakeAxisRotation(horizontal, ry);

  rotMat.Transform(center);
  rotMat.TransformDirection(direction);
  direction.Normalize();

  // ===========================================
  // ASSIGNMENT 3: Fix any other affected values
  // ===========================================


}
