//
// bucket.cpp
//
// Copyright (c) 2013 Matt Pharr
// Stanford cs348b, Assignment 1
//

#include "mbrast.h"

Bucket::Bucket(int extent, int spp) 
    : kExtent(extent), kSamplesPerPixel(spp) {
    int nPixels = extent * extent;
    // FIXME: align this
    samples = new float[4 * nPixels * kSamplesPerPixel];
}

Bucket::~Bucket() {
    delete[] samples;
}

void Bucket::Start(int xx0, int yy0, int xx1, int yy1) {
    x0 = xx0;
    y0 = yy0;
    x1 = xx1;
    y1 = yy1;

    float *sp = samples;
    int nPixels = kExtent * kExtent;
    for (int i = 0; i < nPixels; ++i) {
        // Clear RGB
        for (int j = 0; j < 3 * kSamplesPerPixel; ++j)
            *sp++ = 0.5;
        // Clear Z
        for (int j = 0; j < kSamplesPerPixel; ++j)
            *sp++ = INFINITY;
    }
}


void Bucket::Resolve(uint8_t *image, int xRes, int yRes) {
    float invSamples = 1.f / kSamplesPerPixel;

    for (int y = y0; y < y1; ++y) {
        if (y >= yRes)
            break;
        for (int x = x0; x < x1; ++x) {
            if (x >= xRes)
                continue;

            float *ptr = R(x, y, 0);
            float r = 0;
            for (int i = 0; i < kSamplesPerPixel; ++i)
                r += *ptr++;

            float g = 0;
            for (int i = 0; i < kSamplesPerPixel; ++i)
                g += *ptr++;

            float b = 0;
            for (int i = 0; i < kSamplesPerPixel; ++i)
                b += *ptr++;

            // Normalize
            r *= invSamples;
            g *= invSamples;
            b *= invSamples;

#if 0
            // Gamma correction 
            r = powf(r, .45f); // 1/2.2
            g = powf(g, .45f); // 1/2.2
            b = powf(b, .45f); // 1/2.2
#endif
            
            // Scale and clamp to [0,255]
            r *= 255.f;
            g *= 255.f;
            b *= 255.f;
            if (r < 0.) r = 0;
            else if (r > 255.f) r = 255.f;
            if (g < 0.) g = 0;
            else if (g > 255.f) g = 255.f;
            if (b < 0.) b = 0;
            else if (b > 255.f) b = 255.f;

            image[3*(y*xRes+x)]   = (uint8_t)r;
            image[3*(y*xRes+x)+1] = (uint8_t)g;
            image[3*(y*xRes+x)+2] = (uint8_t)b;
        }
    }
}


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
