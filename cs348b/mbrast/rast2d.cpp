//
// rast2d.cpp
//
// Copyright (c) 2013 Matt Pharr
// Stanford cs348b, Assignment 1
//

#include "mbrast.h"
#include <stdio.h>

class Rasterizer2D : public Rasterizer {
public:
    bool IsThreadSafe() const {
        return true;
    }

    void PreprocessGrids(std::vector<ShadedGrid> &grids,
                         int bucketEdgeLength, int xRes, int yRes);
    void Rasterize(const std::vector<ShadedGrid> &grids,
                   int numIntervals, Bucket *bucket);
};

Rasterizer *Create2DRasterizer() {
    return new Rasterizer2D;
}

void
Rasterizer2D::PreprocessGrids(std::vector<ShadedGrid> &grids,
                              int bucketEdgeLength, int xRes, int yRes) {
    for (unsigned int i = 0; i < grids.size(); ++i) {
        ShadedGrid &sg = grids[i];

        const float * x0_buffer = sg.x0();
        const float * y0_buffer = sg.y0();
        const float * w0_buffer = sg.w0();

        int j;
        int size = sg.nu*sg.nv;

        float xmin = x0_buffer[0] / w0_buffer[0];
        float ymin = y0_buffer[0] / w0_buffer[0];
        float xmax = x0_buffer[size - 1] / w0_buffer[size - 1];
        float ymax = y0_buffer[size - 1] / w0_buffer[size - 1];

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
        sg.box.Xmin = floorf(xmin);
        sg.box.Ymin = floorf(ymin);
        sg.box.Xmax = ceilf(xmax);
        sg.box.Ymax = ceilf(ymax);
    }
}


// Rasterize the triangle with vertices (u0,v0), (u1,v1), (u2,v2) in the
// given bucket.
static void
RasterizeTriangle(const ShadedGrid &sg, int u0, int v0, int u1, int v1,
                  int u2, int v2, Bucket *bucket) {
    // Compute offsets in the vertex arrays for the three vertices of the
    // triangle to be rasterized
    int offset0 = sg.nu * v0 + u0;
    int offset1 = sg.nu * v1 + u1;
    int offset2 = sg.nu * v2 + u2;

    // Compute screen-space vertex positions of the triangle at start time;
    // ignore motion completely.
    float x0 = sg.x0()[offset0] / sg.w0()[offset0];
    float y0 = sg.y0()[offset0] / sg.w0()[offset0];
    float x1 = sg.x0()[offset1] / sg.w0()[offset1];
    float y1 = sg.y0()[offset1] / sg.w0()[offset1];
    float x2 = sg.x0()[offset2] / sg.w0()[offset2];
    float y2 = sg.y0()[offset2] / sg.w0()[offset2];

    // Now compute the bounding box of the triangle on the screen in
    // floating-point pixel coordinates.
    float xMin = std::min(x0, std::min(x1, x2));
    float xMax = std::max(x0, std::max(x1, x2));
    float yMin = std::min(y0, std::min(y1, y2));
    float yMax = std::max(y0, std::max(y1, y2));

    // Compute integer pixel bounds, clamped to the bucket extent
    int ixMin = std::max((int)floorf(xMin), bucket->x0);
    int ixMax = std::min((int)ceilf(xMax),  bucket->x1);
    int iyMin = std::max((int)floorf(yMin), bucket->y0);
    int iyMax = std::min((int)ceilf(yMax),  bucket->y1);

    // Triangle setup.
    // Compute the edge equation coefficients.
    float edge_a_0 = -(y1 - y0);
    float edge_a_1 = -(y2 - y1);
    float edge_a_2 = -(y0 - y2);

    float edge_b_0 =   x1 - x0;
    float edge_b_1 =   x2 - x1;
    float edge_b_2 =   x0 - x2;

    float edge_c_0 = edge_a_0 * -x0 + edge_b_0 * -y0;
    float edge_c_1 = edge_a_1 * -x1 + edge_b_1 * -y1;
    float edge_c_2 = edge_a_2 * -x2 + edge_b_2 * -y2;

    // Triangle area and inverse area
    float area = 0.5f * ((x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0));
    float inv2Area = 1.f / (2.f * area);

    if (area <= 0.)
        // The triangle is backfacing.
        return;

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
                // Evaluate edge equations at sample position (sx,sy).
                float sx = xSamples[sampleNum], sy = ySamples[sampleNum];
                float e0 = edge_a_0 * sx + edge_b_0 * sy + edge_c_0;
                float e1 = edge_a_1 * sx + edge_b_1 * sy + edge_c_1;
                float e2 = edge_a_2 * sx + edge_b_2 * sy + edge_c_2;

                if (e0 <= 0.f || e1 <= 0.f || e2 <= 0.f)
                    // The sample is outside the triangle
                    continue;

                // Compute interpolation weights
                float wt0 = e1 * inv2Area;
                float wt1 = e2 * inv2Area;
                float wt2 = e0 * inv2Area;

                // Interpolate z
                float z = (wt0 * sg.z0()[offset0] +
                           wt1 * sg.z0()[offset1] +
                           wt2 * sg.z0()[offset2]);

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
Rasterizer2D::Rasterize(const std::vector<ShadedGrid> &grids,
                        int numIntervals, Bucket *bucket) {
    for (unsigned int i = 0; i < grids.size(); ++i) {
        const ShadedGrid &sg = grids[i];

        if (!OverLapRect(sg.box.Xmin, sg.box.Ymin, sg.box.Xmax, sg.box.Ymax,
                        bucket->x0, bucket->y0, bucket->x1, bucket->y1))
        {
            continue;
        }

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

