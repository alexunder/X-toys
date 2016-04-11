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
    float xa = sg.x0()[offset0] / sg.w0()[offset0];
    float ya = sg.y0()[offset0] / sg.w0()[offset0];
    float xb = sg.x0()[offset1] / sg.w0()[offset1];
    float yb = sg.y0()[offset1] / sg.w0()[offset1];
    float xc = sg.x0()[offset2] / sg.w0()[offset2];
    float yc = sg.y0()[offset2] / sg.w0()[offset2];

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

	// Now compute the bounding box of the triangle on the screen in
    // floating-point pixel coordinates.
    float xMin = std::min(xa, std::min(xb, xc));
    float xMax = std::max(xa, std::max(xb, xc));
    float yMin = std::min(ya, std::min(yb, yc));
    float yMax = std::max(ya, std::max(yb, yc));

    // Compute integer pixel bounds, clamped to the bucket extent
    int ixMin = std::max((int)floorf(xMin), bucket->x0);
    int ixMax = std::min((int)ceilf(xMax),  bucket->x1);
    int iyMin = std::max((int)floorf(yMin), bucket->y0);
    int iyMax = std::min((int)ceilf(yMax),  bucket->y1);

    // Triangle setup.
    // Compute the edge equation coefficients.
    /*
    float edge_a_0 = -(y1 - y0);
    float edge_a_1 = -(y2 - y1);
    float edge_a_2 = -(y0 - y2);

    float edge_b_0 =   x1 - x0;
    float edge_b_1 =   x2 - x1;
    float edge_b_2 =   x0 - x2;

    float edge_c_0 = edge_a_0 * -x0 + edge_b_0 * -y0;
    float edge_c_1 = edge_a_1 * -x1 + edge_b_1 * -y1;
    float edge_c_2 = edge_a_2 * -x2 + edge_b_2 * -y2;
    */


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
                int i;

                for (i = 0; i <= interval; i++)
                {
                    //Get the current vertex
                    float t = (float)i /(float)interval;

					float point_a_xt = (1 - t) * point_a_x0 + t * point_a_x1;
					float point_a_yt = (1 - t) * point_a_y0 + t * point_a_y1;
					float point_a_zt = (1 - t) * point_a_z0 + t * point_a_z1;
					float point_a_wt = (1 - t) * point_a_w0 + t * point_a_w1;

					float point_b_xt = (1 - t) * point_b_x0 + t * point_b_x1;
					float point_b_yt = (1 - t) * point_b_y0 + t * point_b_y1;
					float point_b_zt = (1 - t) * point_b_z0 + t * point_b_z1;
					float point_b_wt = (1 - t) * point_b_w0 + t * point_b_w1;

					float point_c_xt = (1 - t) * point_c_x0 + t * point_c_x1;
					float point_c_yt = (1 - t) * point_c_y0 + t * point_c_y1;
					float point_c_zt = (1 - t) * point_c_z0 + t * point_c_z1;
					float point_c_wt = (1 - t) * point_c_w0 + t * point_c_w1;

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
					float sx = xSamples[sampleNum], sy = ySamples[sampleNum];
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
}

void
Rasterizer3D::Rasterize(const std::vector<ShadedGrid> &grids,
                        int numIntervals, Bucket *bucket) {
    int i;

    for (i = 0; i < grids.size(); ++i)
    {
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
                RasterizeTriangle(sg, u, v, u+1, v, u+1, v+1, numIntervals, bucket);
                RasterizeTriangle(sg, u, v, u+1, v+1, u, v+1, numIntervals, bucket);
            }
        }
    }
}
