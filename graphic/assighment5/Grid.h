/*
 * The Grid class derived from Object3D.
 * Implemented by Mohism Research
 */

#ifndef __H_GRID
#define __H_GRID

#include "Object3D.h"
#include "BoundingBox.h"

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
private:
    int mXSize;
    int mYSize;
    int mZSize;
    bool * mpFlagArray;
};

#endif
