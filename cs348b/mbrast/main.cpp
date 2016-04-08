//
// main.cpp
//
// Copyright (c) 2013 Matt Pharr
// Stanford cs348b, Assignment 1
//

#include "mbrast.h"
#include "timing.h"
#include "parallel.h"
#include <algorithm>
#include <stdio.h>
#include <strings.h>

#define MAX_CORES 16

bool OverLapRect(int ax0, int ay0, int ax1, int ay1,
                 int bx0, int by0, int bx1, int by1)
{
    int ixMin = std::max(ax0, bx0);
    int ixMax = std::min(ax1, bx1);
    int iyMin = std::max(ay0, by0);
    int iyMax = std::min(ay1, by1);

    if (ixMin >= ixMax || iyMin >= iyMax)
        return false;

    return true;
}

bool ReadGrids(const char *fn, std::vector<ShadedGrid> *grids);
void WritePPM(const uint8_t *image, int xRes, int yRes, const char *fn);
bool CompareToGolden(const uint8_t *image, int xRes, int yRes,
                     const char *fn, const char *nameBase);

struct RastTask : public Task {
    RastTask(Rasterizer *r, Bucket **b, int xx0, int yy0, int xx1, int yy1,
             int ni, const std::vector<ShadedGrid> &g, uint8_t *im, int xr,
             int yr)
        : x0(xx0), y0(yy0), x1(xx1), y1(yy1), nIntervals(ni),
          grids(g), xRes(xr), yRes(yr) {
        rast = r;
        buckets = b;
        image = im;
    }

    void Run(int threadIndex) {
        assert(threadIndex < MAX_CORES);
        Bucket *bucket = buckets[threadIndex];
        bucket->Start(x0, y0, x1, y1);
        rast->Rasterize(grids, nIntervals, bucket);
        bucket->Resolve(image, xRes, yRes);
    }

    Rasterizer *rast;
    Bucket **buckets;
    const int x0, y0, x1, y1;
    const int nIntervals;

    const std::vector<ShadedGrid> &grids;
    uint8_t *image;
    const int xRes, yRes;
};


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: mbrast [grid_file.out]\n");
        return 1;
    }

    // Read grids from grid dump file
    std::vector<ShadedGrid> grids;
    if (!ReadGrids(argv[1], &grids))
        return 1;

    InitSamples();

    // Extract the name of the scene in case a full pathname was provided
    // on the command line
    char *sceneName = rindex(argv[1], '/');
    if (sceneName != NULL)
        ++sceneName;
    else
        sceneName = argv[1];

    // Remove trailing ".grids" from scene name
    char *end = rindex(sceneName, '.');
    assert(end);
    *end = '\0';

    printf("Rendering scene \"%s\"\n", sceneName);

    // Change this call to switch to creating an instance of your 3D
    // rasterizer once you start implementing it.
    Rasterizer *rast = Create2DRasterizer();
    //Rasterizer *rast = Create3DRasterizer();

    // Initialize task system for multithreading if the rasterizer
    // indicates that it's thread safe.
    bool useTasks = rast->IsThreadSafe();
    int nThreads = NumSystemCores();
    assert(nThreads < MAX_CORES);
    if (useTasks) {
        printf("Using %d threads!\n", nThreads);
        TasksInit();
    }

    // Render the scene multiple times, with each of the pixelSamples
    // sample counts.  The sample counts must all be powers of two.
    // The bucketSize array, which must be the same size as
    // pixelSamples[], gives the length of the side of the bucket we'll
    // decompose the image into for the corresponding sample count.
    int pixelSamples[] = { 1, 4, 32 };
    int bucketSize[] = { 64, 32, 8 };
    assert(sizeof(pixelSamples) == sizeof(bucketSize));
    int samplesSize = sizeof(pixelSamples) / sizeof(pixelSamples[0]);

    int failures = 0;
    for (int samplesIndex = 0; samplesIndex < samplesSize; ++samplesIndex) {
        assert(pixelSamples[samplesIndex] <= MAX_SAMPLES_PER_PIXEL);
        int nPixelSamples = pixelSamples[samplesIndex];
        int bucketExtent = bucketSize[samplesIndex];

        printf("Rendering with %d samples per pixel\n", nPixelSamples);

        // 720p resolution.  (This can't be easily changed, as it's baked
        // into the xy coordinates of the micropolygons.  So changing this
        // here would require a corresponding adjustment to those
        // coordinate values when the grids are read in....)
        int xRes = 1280, yRes = 720;

        // Allow the rasterizer to preprocess the grids
        ResetAndStartTimer();
        rast->PreprocessGrids(grids, bucketExtent, xRes, yRes);
        double preprocessCycles = GetElapsedMCycles() / 1024.;
        printf("  Spent %.3f billion cycles on grid preprocessing.\n",
               preprocessCycles);

        // Allocate final output image as well as one Bucket structure for
        // each rasterization task thread we're running; this way the
        // rasterizer can update the framebuffer in its Bucket directly
        // without needing to coordinate with any other rasterizer threads.
        uint8_t *image = new uint8_t[xRes * yRes * 3];
        Bucket *buckets[MAX_CORES];
        int nBuckets = useTasks ? nThreads : 1;
        for (int i = 0; i < nBuckets; ++i)
            buckets[i] = new Bucket(bucketExtent, nPixelSamples);

        // Render with intervals set from 1 to the number of pixel samples
        // in multiples of two.
        for (int logIntervals = 0; (1 << logIntervals) <= nPixelSamples;
             ++logIntervals) {
            int nIntervals = (1 << logIntervals);

            printf("  Rendering with %d intervals: ", nIntervals);
            fflush(stdout);

            if (useTasks) {
                // Create a RastTask instance for each of the buckets of
                // the image; do this outside of the timer so as not to
                // penalize for the unnecessary dynamic allocation overhead
                // in the implementation here.
                std::vector<Task *> rastTasks;
                for (int y = 0; y < yRes; y += bucketExtent) {
                    for (int x = 0; x < xRes; x += bucketExtent) {
                        int x1 = std::min(x + bucketExtent, xRes);
                        int y1 = std::min(y + bucketExtent, yRes);
                        RastTask *rt = new RastTask(rast, &buckets[0], x, y,
                                                    x1, y1, nIntervals, grids,
                                                    image, xRes, yRes);
                        rastTasks.push_back(rt);
                    }
                }

                ResetAndStartTimer();
                EnqueueTasks(rastTasks);
                WaitForAllTasks();
            }
            else {
                // If not using tasks, just loop over all of the buckets
                // and rasterize.
                ResetAndStartTimer();
                for (int y = 0; y < yRes; y += bucketExtent) {
                    for (int x = 0; x < xRes; x += bucketExtent) {
                        buckets[0]->Start(x, y, std::min(x + bucketExtent, xRes),
                                          std::min(y + bucketExtent, yRes));
                        rast->Rasterize(grids, nIntervals, buckets[0]);
                        buckets[0]->Resolve(image, xRes, yRes);
                    }
                }
            }

            // Report total time for rasterization
            double rastGCycles = GetElapsedMCycles() / 1024.;
            printf("  %.3f billion cycles\n", rastGCycles);

            // Write the image
            char outName[256];
            sprintf(outName, "%s_int%d_samp%d.ppm", sceneName, nIntervals,
                    nPixelSamples);
            WritePPM(image, xRes, yRes, outName);
            printf("  Wrote output image \"%s\"\n", outName);

            // Compare to the "golden" image for correctness
            char goldenName[256];
            sprintf(goldenName, "golden/%s_%d.ppm", sceneName,
                    nPixelSamples);
            if (!CompareToGolden(image, xRes, yRes, goldenName, outName))
                ++failures;
        }
        printf("\n");

        for (int i = 0; i < nBuckets; ++i)
            delete buckets[i];

        delete[] image;
    }

    if (useTasks)
        TasksCleanup();

    delete rast;
    return failures;
}


bool
ReadGrids(const char *fn, std::vector<ShadedGrid> *grids) {
    FILE *f = fopen(fn, "rb");
    if (!f) {
        perror(fn);
        return false;
    }

    int nMps = 0;
    int nu, nv;
    while (fread(&nu, sizeof(int), 1, f) == 1) {
        fread(&nv, sizeof(int), 1, f);
        int nFloats = (nu * nv) * 11; // xyzw xyzw rgb == 11 values / vertex
        ShadedGrid grid;
        grid.nu = nu;
        grid.nv = nv;
        grid.buf = new float[nFloats];

        nMps += (nu-1) * (nv-1);

        if (fread(grid.buf, sizeof(float), nFloats, f) != (size_t)nFloats) {
            fprintf(stderr, "Premature EOF in grid file \"%s\"!\n", fn);
            return false;
        }

        grids->push_back(grid);
    }

    printf("Read %d grids with %d micropolygons from \"%s\"\n",
           (int)grids->size(), nMps, fn);
    fclose(f);
    return true;
}


void
WritePPM(const uint8_t *image, int xRes, int yRes, const char *fn) {
    FILE *f = fopen(fn, "wb");
    if (!f) {
        perror(fn);
        return;
    }

    fprintf(f, "P6\n%d %d 255\n", xRes, yRes);
    if (fwrite(image, sizeof(uint8_t), 3*xRes*yRes, f) != (size_t)(3*xRes*yRes))
        perror(fn);
    fclose(f);
}


bool
CompareToGolden(const uint8_t *image, int xRes, int yRes, const char *fn,
                const char *nameBase) {
    FILE *f = fopen(fn, "rb");
    if (!f) {
        perror(fn);
        return false;
    }

    int xr, yr;
    if (fscanf(f, "P6\n%d %d 255\n", &xr, &yr) != 2) {
        fprintf(stderr, "Error reading golden image \"%s\"\n", fn);
        return false;
    }

    if (xr != xRes || yr != yRes) {
        fprintf(stderr, "Golden image resolution (%d,%d) doesn't match "
                "rendered image resolution (%d,%d)\n", xr, yr, xRes, yRes);
        return false;
    }

    uint8_t *golden = new uint8_t[3*xRes*yRes];
    if (fread(golden, sizeof(uint8_t), 3*xRes*yRes, f) != (size_t)3*xRes*yRes) {
        perror(fn);
        return false;
    }

    uint8_t *diffImg = new uint8_t[3*xRes*yRes];
    int smallDiffs = 0, bigDiffs = 0;
    for (int i = 0; i < 3 * xRes * yRes; ++i) {
        int diff = abs((int)golden[i] - (int)image[i]);
        diffImg[i] = (uint8_t)diff;
        if (diff > 4)
            ++bigDiffs;
        else if (diff > 0)
            ++smallDiffs;
    }

    if (bigDiffs != 0 || smallDiffs != 0) {
        char diffName[128];
        sprintf(diffName, "diff_%s", nameBase);
        WritePPM(diffImg, xRes, yRes, diffName);
        printf("  Wrote diff image \"%s\"\n", diffName);
    }

    bool legit = (bigDiffs < 2048 && smallDiffs < 32768);
    printf("  Image diffs: %d small, %d big. %s\n", smallDiffs, bigDiffs,
           legit ? "Passed." : "FAILED");
    return legit;
}
