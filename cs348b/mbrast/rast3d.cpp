//
// rast3d.cpp
//
// Stanford cs348b, Assignment 1
//

#include "mbrast.h"

class Rasterizer3D : public Rasterizer {
public:
    bool IsThreadSafe() const {
        // TODO: update this as appropriate
        return false;
    }

    void PreprocessGrids(std::vector<ShadedGrid> &grids,
                         int bucketEdgeLength, int xRes, int yRes);    
     void Rasterize(const std::vector<ShadedGrid> &grids,
                   int numIntervals, Bucket *bucket);
};

Rasterizer *Create3DRasterizer() {
    return new Rasterizer3D;
}

void
Rasterizer3D::PreprocessGrids(std::vector<ShadedGrid> &grids,
                              int bucketEdgeLength, int xRes, int yRes) {
    // TODO: implement this 
}

void
Rasterizer3D::Rasterize(const std::vector<ShadedGrid> &grids,
                        int numIntervals, Bucket *bucket) {
    // TODO: implement this
}
