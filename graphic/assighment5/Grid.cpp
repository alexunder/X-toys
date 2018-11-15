/*
 * The Grid class derived from Object3D.
 * Implemented by Mohism Research
 */

#include "Grid.h"
#include "MarchingInfo.h"
#include "ray.h"
#include "hit.h"
#include "PhongMaterial.h"
#include <memory.h>

Grid::Grid(BoundingBox *bb, int nx, int ny, int nz)
    : mXSize(nx), mYSize(ny), mZSize(nz)
{
    mpBox = bb;
    int axis;
    int gridSize[3];
    gridSize[0] = mXSize;
    gridSize[1] = mYSize;
    gridSize[2] = mZSize;
    for (axis = 0; axis < 3; axis++)
    {
        mVoxel[axis] = fabs(mpBox->getMax()[axis] - mpBox->getMin()[axis])
            / gridSize[axis];
    }

    int objectSize = mXSize * mYSize * mZSize;
    mObjects = new Object3DVector[objectSize];
}

Grid::~Grid()
{
    if (mObjects != NULL)
        delete [] mObjects;
}

void Grid::paint(void)
{
    int i;
    int j;
    int k;

    if (mpBox == NULL)
        return;
    printf("Grid::paint entry\n");
    Vec3f minp = mpBox->getMin();
    Vec3f maxp = mpBox->getMax();

    float xBox = maxp.x() - minp.x();
    float yBox = maxp.y() - minp.y();
    float zBox = maxp.z() - minp.z();

    int xSize = getXSize();
    int ySize = getYSize();
    int zSize = getZSize();

    float xDelta = xBox / xSize;
    float yDelta = yBox / ySize;
    float zDelta = zBox / zSize;

    for (k = 0; k < zSize; k++)
    for (j = 0; j < ySize; j++)
    for (i = 0; i < xSize; i++)
    {
        printf("Obj:%d %d %d, num=%d\n", i, j, k, mObjects[offset(i, j, k)].getNumObjects());
        if (mObjects[offset(i, j, k)].getNumObjects() > 0) {
            printf("Paint:%d, %d, %d\n", i, j, k);
            Vec3f curMinPoint(minp.x() + i*xDelta,
                              minp.y() + j*yDelta,
                              minp.z() + k*zDelta );

            Vec3f curMaxPoint(curMinPoint.x() + xDelta,
                              curMinPoint.y() + yDelta,
                              curMinPoint.z() + zDelta );

            glBegin(GL_QUADS);

            //The up plane
            Vec3f normalUp;
            Vec3f::Cross3( normalUp,
                           Vec3f(0.0, curMaxPoint.y() - curMinPoint.y() , 0.0),
                           Vec3f(curMaxPoint.x() - curMinPoint.x(), 0.0, 0.0) );
            glNormal3f(normalUp.x(), normalUp.y(), normalUp.z());

            glVertex3f(curMinPoint.x(), curMinPoint.y(), curMaxPoint.z());
            glVertex3f(curMaxPoint.x(), curMinPoint.y(), curMaxPoint.z());
            glVertex3f(curMaxPoint.x(), curMaxPoint.y(), curMaxPoint.z());
            glVertex3f(curMinPoint.x(), curMaxPoint.y(), curMaxPoint.z());

            //the down plane
            Vec3f normalDown;
            Vec3f::Cross3( normalDown,
                           Vec3f(curMaxPoint.x() - curMinPoint.x(), 0.0, 0.0),
                           Vec3f(0.0, curMaxPoint.y() - curMinPoint.y(), 0.0) );
            glNormal3f(normalDown.x(), normalDown.y(), normalDown.z());

            glVertex3f(curMinPoint.x(), curMinPoint.y(), curMinPoint.z());
            glVertex3f(curMaxPoint.x(), curMinPoint.y(), curMinPoint.z());
            glVertex3f(curMaxPoint.x(), curMaxPoint.y(), curMinPoint.z());
            glVertex3f(curMinPoint.x(), curMaxPoint.y(), curMinPoint.z());

            //the front plane
            Vec3f normalFront;
            Vec3f::Cross3( normalFront,
                           Vec3f(0.0, curMaxPoint.y() - curMinPoint.y(), 0.0),
                           Vec3f(0.0, 0.0, curMaxPoint.z() - curMinPoint.z()) );
            glNormal3f(normalFront.x(), normalFront.y(), normalFront.z());

            glVertex3f(curMinPoint.x(), curMinPoint.y(), curMinPoint.z());
            glVertex3f(curMinPoint.x(), curMinPoint.y(), curMaxPoint.z());
            glVertex3f(curMinPoint.x(), curMaxPoint.y(), curMaxPoint.z());
            glVertex3f(curMinPoint.x(), curMaxPoint.y(), curMinPoint.z());

            //the back plane
            Vec3f normalBack;
            Vec3f::Cross3( normalBack,
                           Vec3f(0.0, 0.0, curMaxPoint.z() - curMinPoint.z()),
                           Vec3f(0.0, curMaxPoint.y() - curMinPoint.y(), 0.0) );
            glNormal3f(normalBack.x(), normalBack.y(), normalBack.z());

            glVertex3f(curMaxPoint.x(), curMinPoint.y(), curMinPoint.z());
            glVertex3f(curMaxPoint.x(), curMinPoint.y(), curMaxPoint.z());
            glVertex3f(curMaxPoint.x(), curMaxPoint.y(), curMaxPoint.z());
            glVertex3f(curMaxPoint.x(), curMaxPoint.y(), curMinPoint.z());

            //the left plane
            Vec3f normalLeft;
            Vec3f::Cross3( normalLeft,
                           Vec3f(curMinPoint.x() - curMaxPoint.x(), 0.0, 0.0),
                           Vec3f(0.0, 0.0, curMaxPoint.z() - curMinPoint.z()) );
            glNormal3f(normalLeft.x(), normalLeft.y(), normalLeft.z());

            glVertex3f(curMaxPoint.x(), curMinPoint.y(), curMinPoint.z());
            glVertex3f(curMaxPoint.x(), curMinPoint.y(), curMaxPoint.z());
            glVertex3f(curMinPoint.x(), curMinPoint.y(), curMaxPoint.z());
            glVertex3f(curMinPoint.x(), curMinPoint.y(), curMinPoint.z());

            //the right plane
            Vec3f normalRight;
            Vec3f::Cross3( normalRight,
                           Vec3f(0.0, 0.0, curMaxPoint.z() - curMinPoint.z()),
                           Vec3f(curMinPoint.x() - curMaxPoint.x(), 0.0, 0.0) );
            glNormal3f(normalRight.x(), normalRight.y(), normalRight.z());

            glVertex3f(curMaxPoint.x(), curMaxPoint.y(), curMinPoint.z());
            glVertex3f(curMaxPoint.x(), curMaxPoint.y(), curMaxPoint.z());
            glVertex3f(curMinPoint.x(), curMaxPoint.y(), curMaxPoint.z());
            glVertex3f(curMinPoint.x(), curMaxPoint.y(), curMinPoint.z());

            glEnd();
        }
    }
    printf("Grid::paint Exit\n");
}

int Grid::posToVoxel(const Vec3f & point, int axis) const
{
   int v = (int)((point[axis] - mpBox->getMin()[axis])
               / mVoxel[axis]);
   return v;
}

float Grid::voxelToPos(int p, int axis) const {
    return mpBox->getMin()[axis] + p * mVoxel[axis];
}

void Grid::initializeRayMarch(MarchingInfo &mi, const Ray &r, float tmin) const
{
    Vec3f rayOrigin = r.getOrigin();
    Vec3f rayDir = r.getDirection();

    Vec3f boxMin = mpBox->getMin();
    Vec3f boxMax = mpBox->getMax();

    float rayT;
    if (mpBox->Inside(rayOrigin))
        rayT = 0.0;
    else if (!mpBox->IntersectP(r, &rayT))
    {
        return;
    }
    
    printf("rayT=%f\n", rayT);
    Vec3f gridIntersect = r.pointAtParameter(rayT);
    cout << "gridIntersect=" << gridIntersect << endl;
    cout << "Box min=" << mpBox->getMin() << endl;
    printf("mVoxel:%d, %d, %d \n", mVoxel[0], mVoxel[1], mVoxel[2]);
    //MarchingInfo for indices
    int pos[3];
    int axis;
    int sign[3];
    float delta[3];
    float next[3];
    int indices[3];
    for (axis = 0; axis < 3; axis++) {
        pos[axis] = posToVoxel(gridIntersect, axis);
        if (rayDir[axis] != 0)
            sign[axis] = rayDir[axis] > 0 ? 1 : -1;

        if (sign[axis] >= 0) {
            next[axis] = rayT +
                (voxelToPos(pos[axis]+1, axis) - gridIntersect[axis])/rayDir[axis];
            delta[axis] = mVoxel[axis] / rayDir[axis];
        }
        else {
            next[axis] = rayT +
                (voxelToPos(pos[axis], axis) - gridIntersect[axis])/rayDir[axis];
            delta[axis] = mVoxel[axis] / rayDir[axis];
        }
    }
    printf("Indices:%d, %d, %d \n", pos[0], pos[1], pos[2]);
    mi.setIndices(pos[0], pos[1], pos[2]);
    mi.setSign(sign[0], sign[1], sign[2]);
    mi.setDelta(delta[0], delta[1], delta[2]);
    mi.setNext(next[0], next[1], next[2]);
    mi.set_tmin(rayT);
}

bool Grid::intersect(const Ray &r, Hit &h, float tmin)
{
    //Prepare the Material objects
    Vec3f diffuseColor(1, 1, 1);
    Vec3f specularColor(0, 0, 0);
    float exponent = 1;
    Vec3f reflectiveColor(0, 0, 0);
    Vec3f transparentColor(0, 0, 0);
    float indexOfRefraction = 1;
    PhongMaterial *m = new PhongMaterial(diffuseColor, specularColor, exponent,
            reflectiveColor, transparentColor, indexOfRefraction);

    //Start to do DDA
    MarchingInfo mi;
    initializeRayMarch(mi, r, tmin);

    bool hitSomething = false;
    int i, j, k;
    do {
        mi.getIndices(i, j, k);
        
        printf("The current point is:%d,%d,%d\n", i, j, k);

        if (mObjects[offset(i, j, k)].getNumObjects() > 0) {
            h.set(mi.get_tmin(), m, mi.getNormal(), r);
            return true;
        }

        mi.nextCell();
    } while(i < mXSize && j < mYSize && k < mZSize);
    return false;
}

void Grid::AddObjectToGrid(Object3D * obj, int i, int j, int k) {
    mObjects[offset(i, j, k)].addObject(obj);
}

void Grid::dumpObjectInfo() {
    printf("The grid object details are:\n");
    int i;
    int size = mXSize*mYSize*mZSize;
    for (i = 0; i < size; i++) {
        printf("Object:%d, Objects'size=%d\n", i, mObjects[i].getNumObjects());
    }
}
