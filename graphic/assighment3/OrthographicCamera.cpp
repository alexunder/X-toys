/*
 * The implementation of the OrthographicCamera class
 * Implemented by Mohism Research
 */

#include "OrthographicCamera.h"

#define T_MIN -100000.0

OrthographicCamera::OrthographicCamera(const Vec3f &center, Vec3f &direction, Vec3f &up, int size)
    : mCenter(center), mDirection(direction), mUp(up), mCameraSize(size)
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
#ifdef DEBUG
    cout<<"mCenter="<<mCenter<<endl;
    cout<<"mDirection="<<mDirection<<endl;
    cout<<"mUp="<<mUp<<endl;
    cout<<"mHorizontal="<<mHorizontal<<endl;
#endif
}

Ray OrthographicCamera::generateRay(Vec2f point)
{
    float x = point.x();
    float y = point.y();

    float l = - mCameraSize / 2.0;
    float t = - mCameraSize / 2.0;

    float u = x * mCameraSize + l;
    float v = y * mCameraSize + t;

    Vec3f originalPoint = mCenter + u * mHorizontal + v * mUp;
    Ray r(originalPoint, mDirection);

    return r;
}
    
float OrthographicCamera::getTMin() const
{
    return T_MIN;
}

// ====================================================================
// Create an orthographic camera with the appropriate dimensions that
// crops the screen in the narrowest dimension.
// ====================================================================

void OrthographicCamera::glInit(int w, int h)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (w > h)
    glOrtho(-size/2.0, size/2.0, -size*(float)h/(float)w/2.0, size*(float)h/(float)w/2.0, 0.5, 40.0);
  else
    glOrtho(-size*(float)w/(float)h/2.0, size*(float)w/(float)h/2.0, -size/2.0, size/2.0, 0.5, 40.0);
}

// ====================================================================
// Place an orthographic camera within an OpenGL scene
// ====================================================================

void OrthographicCamera::glPlaceCamera(void)
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

void OrthographicCamera::dollyCamera(float dist)
{
  center += direction*dist;

  // ===========================================
  // ASSIGNMENT 3: Fix any other affected values
  // ===========================================


}

// ====================================================================
// truckCamera: Translate camera perpendicular to the direction vector
// ====================================================================

void OrthographicCamera::truckCamera(float dx, float dy)
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

void OrthographicCamera::rotateCamera(float rx, float ry)
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
  
  // ===========================================
  // ASSIGNMENT 3: Fix any other affected values
  // ===========================================


}
