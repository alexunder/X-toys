//
// rast3d.cpp
//
// Stanford cs348b, Assignment 1
//

#include "mbrast.h"
#include <stdio.h>

class Rasterizer3D : public Rasterizer {
public:
    bool IsThreadSafe() const {
        // TODO: update this as appropriate
        return true;
    }

    void PreprocessGrids(const std::vector<ShadedGrid> &grids,
                         int bucketEdgeLength, int xRes, int yRes);    
    void Rasterize(const std::vector<ShadedGrid> &grids,
                   int numIntervals, Bucket *bucket);
private:
    BBox2D mBox;
};

Rasterizer *Create3DRasterizer() {
    return new Rasterizer3D;
}

void
Rasterizer3D::PreprocessGrids(const std::vector<ShadedGrid> &grids,
                              int bucketEdgeLength, int xRes, int yRes) {
    float xmin = xRes - 1.0;
    float ymin = yRes - 1.0;
    float xmax = 0.0;
    float ymax = 0.0;

    for (unsigned int i = 0; i < grids.size(); ++i) {
        const ShadedGrid &sg = grids[i];

        const float * x0_buffer = sg.x0();
        const float * y0_buffer = sg.y0();
        const float * w0_buffer = sg.w0();

        const float * x1_buffer = sg.x1();
        const float * y1_buffer = sg.y1();
        const float * w1_buffer = sg.w1();

        int j;
        int size = sg.nu*sg.nv;

        for (j = 0; j < size; j++)
        {
            float xTemp = x0_buffer[j] / w0_buffer[j];
            float yTemp = y0_buffer[j] / w0_buffer[j];
            // X min
            if (xTemp < xmin)
                xmin = xTemp;

            //X max
            if (xTemp > xmax)
                xmax = xTemp;

            //Y min
            if (yTemp < ymin)
                ymin = yTemp;

            //Y max
            if (yTemp > ymax)
                ymax = yTemp;

        }

        for (j = 0; j < size; j++)
        {
            float xTemp = x1_buffer[j] / w1_buffer[j];
            float yTemp = y1_buffer[j] / w1_buffer[j];
            // X min
            if (xTemp < xmin)
                xmin = xTemp;

            //X max
            if (xTemp > xmax)
                xmax = xTemp;

            //Y min
            if (yTemp < ymin)
                ymin = yTemp;

            //Y max
            if (yTemp > ymax)
                ymax = yTemp;

        }
    }

    mBox.Xmin = floorf(xmin);
    mBox.Ymin = floorf(ymin);
    mBox.Xmax = ceilf(xmax);
    mBox.Ymax = ceilf(ymax);
}

static void RasterizeTriangle(const ShadedGrid &sg, int u0, int v0, int u1, int v1,
                              int u2, int v2, Bucket *bucket)
{
    // Compute offsets in the vertex arrays for the three vertices of the
    // triangle to be rasterized
    int offset0 = sg.nu * v0 + u0;
    int offset1 = sg.nu * v1 + u1;
    int offset2 = sg.nu * v2 + u2;

    // Compute screen-space vertex positions of the triangle at start time;
    // ignore motion completely.
// Compute screen-space vertex positions of the triangle at start time;
    Point vx0[2], vx1[2], vx2[2];
    vx0[0].set(sg.x0()[offset0], sg.y0()[offset0], sg.z0()[offset0], sg.w0()[offset0]);
    vx0[1].set(sg.x1()[offset0], sg.y1()[offset0], sg.z1()[offset0], sg.w1()[offset0]);
    vx1[0].set(sg.x0()[offset1], sg.y0()[offset1], sg.z0()[offset1], sg.w0()[offset1]);
    vx1[1].set(sg.x1()[offset1], sg.y1()[offset1], sg.z1()[offset1], sg.w1()[offset1]);
    vx2[0].set(sg.x0()[offset2], sg.y0()[offset2], sg.z0()[offset2], sg.w0()[offset2]);
    vx2[1].set(sg.x1()[offset2], sg.y1()[offset2], sg.z1()[offset2], sg.w1()[offset2]);

    // Now compute the bounding box of the triangle on the screen in
    // floating-point pixel coordinates.
    float xMin = vx0[0].x2d(), xMax = vx0[0].x2d(), yMin = vx0[0].y2d(), yMax = vx0[0].y2d();
    for (int i = 0; i < 2; i++)
    {
        xMin = std::min(xMin, std::min(vx0[i].x2d(), std::min(vx1[i].x2d(), vx2[i].x2d())));
        yMin = std::min(yMin, std::min(vx0[i].y2d(), std::min(vx1[i].y2d(), vx2[i].y2d())));
        xMax = std::max(xMax, std::max(vx0[i].x2d(), std::max(vx1[i].x2d(), vx2[i].x2d())));
        yMax = std::max(yMax, std::max(vx0[i].y2d(), std::max(vx1[i].y2d(), vx2[i].y2d())));
    }    

    // Compute integer pixel bounds, clamped to the bucket extent
    int ixMin = std::max((int)floorf(xMin), bucket->x0);
    int ixMax = std::min((int)ceilf(xMax),  bucket->x1);
    int iyMin = std::max((int)floorf(yMin), bucket->y0);
    int iyMax = std::min((int)ceilf(yMax),  bucket->y1);

    // Loop over the bounding box of the pixels that the triangle possibly
    // covers.
    for (int py = iyMin; py < iyMax; ++py) {
        for (int px = ixMin; px < ixMax; ++px) {
            // Get the set of samples for the pixel (px, py)
            float xSamples[MAX_SAMPLES_PER_PIXEL];
            float ySamples[MAX_SAMPLES_PER_PIXEL];
            float timeSamples[MAX_SAMPLES_PER_PIXEL];
            GetSamples(px, py, bucket->kSamplesPerPixel, xSamples, ySamples,
                       timeSamples);

            // Now loop over all of the samples for the pixel and see if
            // each one is inside the triangle.
            for (int sampleNum = 0; sampleNum < bucket->kSamplesPerPixel;
                 ++sampleNum) {
                //Loop over the intervals

                float sx = xSamples[sampleNum];
                float sy = ySamples[sampleNum];
                float st = timeSamples[sampleNum];
                    //Get the current vertex

                float xa, xb, xc, ya, yb, yc, za, zb, zc;
                Point vt0 = vx0[0] * (1 - st) + vx0[1] * st;
                Point vt1 = vx1[0] * (1 - st) + vx1[1] * st;
                Point vt2 = vx2[0] * (1 - st) + vx2[1] * st;
                xa = vt0.x2d(); 
                ya = vt0.y2d();

                xb = vt1.x2d(); 
                yb = vt1.y2d();

                xc = vt2.x2d(); 
                yc = vt2.y2d();

                za = vt0.z;
                zb = vt1.z;
                zc = vt2.z;

                // Triangle setup.
                // Compute the edge equation coefficients.
                float edge_a_0 = -(yb - ya);
                float edge_a_1 = -(yc - yb);
                float edge_a_2 = -(ya - yc);

                float edge_b_0 =   xb - xa;
                float edge_b_1 =   xc - xb;
                float edge_b_2 =   xa - xc;

                float edge_c_0 = edge_a_0 * -xa + edge_b_0 * -ya;
                float edge_c_1 = edge_a_1 * -xb + edge_b_1 * -yb;
                float edge_c_2 = edge_a_2 * -xc + edge_b_2 * -yc;

                // Triangle area and inverse area
                float area = 0.5f * ((xb - xa) * (yc - ya) - (yb - ya) * (xc - xa));
                float inv2Area = 1.f / (2.f * area);

                if (area <= 0.)
                    continue;

                // Evaluate edge equations at sample position (sx,sy).
                float e0 = edge_a_0 * sx + edge_b_0 * sy + edge_c_0;
                float e1 = edge_a_1 * sx + edge_b_1 * sy + edge_c_1;
                float e2 = edge_a_2 * sx + edge_b_2 * sy + edge_c_2;

                if (e0 <= 0.f || e1 <= 0.f || e2 <= 0.f)
                    continue;

                // Compute interpolation weights
                float wt0 = e1 * inv2Area;
                float wt1 = e2 * inv2Area;
                float wt2 = e0 * inv2Area;

                // Interpolate z
                float z = (wt0 * za + wt1 * zb + wt2 * zc);
                // Z-test: is the z value closer than the z value currently
                // in the framebuffer?
                if (z < *bucket->Z(px, py, sampleNum)) {
                    // z-test passed; update z in bucket framebuffer
                    *bucket->Z(px, py, sampleNum) = z;

                    // Interpolate R, G, B
                    float r = (wt0 * sg.r()[offset0] +
                            wt1 * sg.r()[offset1] +
                            wt2 * sg.r()[offset2]);
                    float g = (wt0 * sg.g()[offset0] +
                            wt1 * sg.g()[offset1] +
                            wt2 * sg.g()[offset2]);
                    float b = (wt0 * sg.b()[offset0] +
                            wt1 * sg.b()[offset1] +
                            wt2 * sg.b()[offset2]);

                    // Store r, g, b in framebuffer
                    *bucket->R(px, py, sampleNum) = r;
                    *bucket->G(px, py, sampleNum) = g;
                    *bucket->B(px, py, sampleNum) = b;
                }
            }
        }
    }
}

void
Rasterizer3D::Rasterize(const std::vector<ShadedGrid> &grids,
                        int numIntervals, Bucket *bucket) {

    if (!OverLapRect(mBox.Xmin, mBox.Ymin, mBox.Xmax, mBox.Ymax,
                bucket->x0, bucket->y0, bucket->x1, bucket->y1))
    {
        return;
    }

    int i;

    for (i = 0; i < grids.size(); ++i)
    {
        const ShadedGrid &sg = grids[i];
        // Loop over all micropolygons in the grid; split each one into two
        // triangles and rasterize each of the triangles.
        for (int v = 0; v < sg.nv-1; ++v) {
            for (int u = 0; u < sg.nu-1; ++u) {
                RasterizeTriangle(sg, u, v, u+1, v, u+1, v+1, bucket);
                RasterizeTriangle(sg, u, v, u+1, v+1, u, v+1, bucket);
            }
        }
    }
}
