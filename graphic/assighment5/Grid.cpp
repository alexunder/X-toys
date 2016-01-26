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
