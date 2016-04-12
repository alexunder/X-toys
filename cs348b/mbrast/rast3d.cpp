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

static void RasterizeTriangle(const ShadedGrid &sg, int u0, int v0, int u1, int v1,
                              int u2, int v2, int interval, Bucket *bucket)
{
    // Compute offsets in the vertex arrays for the three vertices of the
    // triangle to be rasterized
    int offset0 = sg.nu * v0 + u0;
    int offset1 = sg.nu * v1 + u1;
    int offset2 = sg.nu * v2 + u2;

    // Compute screen-space vertex positions of the triangle at start time;
    // ignore motion completely.

	//Acquire the shutter open triangle
    float point_a_x0 = sg.x0()[offset0];
    float point_a_y0 = sg.y0()[offset0];
    float point_a_z0 = sg.z0()[offset0];
    float point_a_w0 = sg.w0()[offset0];

	float point_b_x0 = sg.x0()[offset1];
    float point_b_y0 = sg.y0()[offset1];
    float point_b_z0 = sg.z0()[offset1];
    float point_b_w0 = sg.w0()[offset1];

	float point_c_x0 = sg.x0()[offset2];
    float point_c_y0 = sg.y0()[offset2];
    float point_c_z0 = sg.z0()[offset2];
    float point_c_w0 = sg.w0()[offset2];

    float x_0_min = std::min(point_a_x0, std::min(point_b_x0, point_c_x0));
    float x_0_max = std::max(point_a_x0, std::max(point_b_x0, point_c_x0));
    float y_0_min = std::min(point_a_y0, std::min(point_b_y0, point_c_y0));
    float y_0_max = std::max(point_a_y0, std::max(point_b_y0, point_c_y0));
    float w_0_min = std::min(point_a_w0, std::min(point_b_w0, point_c_w0));
    float w_0_max = std::max(point_a_w0, std::max(point_b_w0, point_c_w0));
	//Acquire the shutter close triangle
    float point_a_x1 = sg.x1()[offset0];
    float point_a_y1 = sg.y1()[offset0];
    float point_a_z1 = sg.z1()[offset0];
    float point_a_w1 = sg.w1()[offset0];

	float point_b_x1 = sg.x1()[offset1];
    float point_b_y1 = sg.y1()[offset1];
    float point_b_z1 = sg.z1()[offset1];
    float point_b_w1 = sg.w1()[offset1];

	float point_c_x1 = sg.x1()[offset2];
    float point_c_y1 = sg.y1()[offset2];
    float point_c_z1 = sg.z1()[offset2];
    float point_c_w1 = sg.w1()[offset2];

    float x_1_min = std::min(point_a_x1, std::min(point_b_x1, point_c_x1));
    float x_1_max = std::max(point_a_x1, std::max(point_b_x1, point_c_x1));
    float y_1_min = std::min(point_a_y1, std::min(point_b_y1, point_c_y1));
    float y_1_max = std::max(point_a_y1, std::max(point_b_y1, point_c_y1));
    float w_1_min = std::min(point_a_w1, std::min(point_b_w1, point_c_w1));
    float w_1_max = std::max(point_a_w1, std::max(point_b_w1, point_c_w1));
	// Now compute the bounding box of the triangle on the screen in
    // floating-point pixel coordinates.

    float xMin = std::min(x_0_min, x_1_min);
    float xMax = std::max(x_0_max, x_1_max);
    float yMin = std::min(y_0_min, y_1_min);
    float yMax = std::max(y_0_max, y_1_max);
    float wMin = std::min(w_0_min, w_1_min);
    float wMax = std::max(w_0_max, w_1_max);

    xMin = xMin / wMax;
    xMax = xMax / wMin;
    yMin = yMin / wMax;
    yMax = yMax / wMin;

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

                float point_a_xt = (1 - st) * point_a_x0 + st * point_a_x1;
                float point_a_yt = (1 - st) * point_a_y0 + st * point_a_y1;
                float point_a_zt = (1 - st) * point_a_z0 + st * point_a_z1;
                float point_a_wt = (1 - st) * point_a_w0 + st * point_a_w1;

                float point_b_xt = (1 - st) * point_b_x0 + st * point_b_x1;
                float point_b_yt = (1 - st) * point_b_y0 + st * point_b_y1;
                float point_b_zt = (1 - st) * point_b_z0 + st * point_b_z1;
                float point_b_wt = (1 - st) * point_b_w0 + st * point_b_w1;

                float point_c_xt = (1 - st) * point_c_x0 + st * point_c_x1;
                float point_c_yt = (1 - st) * point_c_y0 + st * point_c_y1;
                float point_c_zt = (1 - st) * point_c_z0 + st * point_c_z1;
                float point_c_wt = (1 - st) * point_c_w0 + st * point_c_w1;

                float xa = point_a_xt / point_a_wt;
                float ya = point_a_yt / point_a_wt;

                float xb = point_b_xt / point_b_wt;
                float yb = point_b_yt / point_b_wt;

                float xc = point_c_xt / point_c_wt;
                float yc = point_c_yt / point_c_wt;

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
                    return;

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
                float z = (wt0 * point_a_zt + wt1 * point_b_zt + wt2 * point_c_zt);
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
    int i;

    for (i = 0; i < grids.size(); ++i)
    {
        const ShadedGrid &sg = grids[i];
#ifdef OPT
		if (!OverLapRect(sg.box.Xmin, sg.box.Ymin, sg.box.Xmax, sg.box.Ymax,
                    bucket->x0, bucket->y0, bucket->x1, bucket->y1))
        {
            continue;
        }
#endif
        // Loop over all micropolygons in the grid; split each one into two
        // triangles and rasterize each of the triangles.
        for (int v = 0; v < sg.nv-1; ++v) {
            for (int u = 0; u < sg.nu-1; ++u) {
                RasterizeTriangle(sg, u, v, u+1, v, u+1, v+1, numIntervals, bucket);
                RasterizeTriangle(sg, u, v, u+1, v+1, u, v+1, numIntervals, bucket);
            }
        }
    }
}
