/*
 * The Grid class derived from Object3D.
 * Implemented by Mohism Research
 */

#include "Grid.h"
#include <memory.h>

Grid::Grid(BoundingBox *bb, int nx, int ny, int nz)
    : mXSize(nx), mYSize(ny), mZSize(nz)
{
    mpBox = bb;
    int arraySize = mXSize * mYSize * mZSize;
    mpFlagArray = new bool[arraySize];

    memset(mpFlagArray, 0, sizeof(bool)* arraySize);
}

Grid::~Grid()
{
    if (mpFlagArray != NULL)
        delete [] mpFlagArray;
}

void Grid::setVoxelFlag(int i, int j, int k)
{
    mpFlagArray[i + j*mXSize + k*mXSize*mYSize] = true;
}


void Grid::paint(void)
{
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
    mpFlagArray[i + j*mXSize + k*mXSize*mYSize] = true;
        if (mpFlagArray[i + j*mXSize + k*mXSize*mYSize])
        {
            Vec3f curMinPoint(minp.x() + i*xDelta,
                           minp.y() + j*yDelta,
                           minp.z() + k*zDelta );

            Vec3f curMaxPoint(curMinPoint.x() + xDelta,
                              curMinPoint.y() + yDelta,
                              curMinPoint.z() + zDelta );
            
            

        }
    }

}
