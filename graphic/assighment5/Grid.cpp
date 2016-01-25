/*
 * The Grid class derived from Object3D.
 * Implemented by Mohism Research
 */

#include "Grid.h"

Grid::Grid(BoundingBox *bb, int nx, int ny, int nz)
    : mXSize(nx), mYSize(ny), mZSize(nz)
{
    mpBox = bb;
    mpFlagArray = new bool[mXSize * mYSize * mZSize]
}

Grid::~Grid()
{
    if (mpFlagArray != NULL)
        delete [] mpFlagArray; 
}
