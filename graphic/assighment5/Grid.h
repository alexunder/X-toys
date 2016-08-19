/*
 * The Grid class derived from Object3D.
 * Implemented by Mohism Research
 */

#ifndef __H_GRID
#define __H_GRID

#include "Object3D.h"
#include "BoundingBox.h"
#include "MarchingInfo.h"

class Grid : public Object3D
{
public:
   Grid(BoundingBox *bb, int nx, int ny, int nz);
   ~Grid();

    void setVoxelFlag(int i, int j, int k);

    int getXSize()
    {
        return mXSize;
    }

    int getYSize()
    {
        return mYSize;
    }

    int getZSize()
    {
        return mZSize;
    }

    void paint();
    void initializeRayMarch(MarchingInfo &mi, const Ray &r, float tmin) const;
    bool intersect(const Ray &r, Hit &h, float tmin);
    int posToVoxel(const Vec3f & point, int axis) const;
    float voxelToPos(int p, int axis) const;

    inline int offset(int x, int y, int z) const {
        return z*mXSize*mYSize + y*mXSize + x;
    }

private:
    int mXSize;
    int mYSize;
    int mZSize;
    int mVoxel[3];
    bool * mpFlagArray;
};

#endif
