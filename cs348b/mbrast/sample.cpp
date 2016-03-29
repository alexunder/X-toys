//
// sample.cpp
//
// Copyright (c) 2013 Matt Pharr
// Stanford cs348b, Assignment 1
//

#include "mbrast.h"
#include <stdint.h>

// Clarberg et al. Hierarchical Stochastic Motion Blur Rasterization
// http://fileadmin.cs.lth.se/graphics/research/papers/2011/mbtraversal/

inline void GetXY(uint32_t j, const uint32_t m, uint32_t& x, uint32_t& y) {
    x = y = 0;
    uint32_t c1 = 0x3, c2 = 0x1 << (m-1);
    for (j <<= 32-m; j != 0; j <<= 1) {
        if (j & 1u<<31) { // Add matrix columns (XOR)
            x ^= c1 >> 1;
            y ^= c2;
        }
        c1 ^= c1 << 1; // Update matrix columns
        c2 ^= c2 >> 1;
    }
}

static float xs[MAX_SAMPLES_PER_PIXEL], ys[MAX_SAMPLES_PER_PIXEL];

// Precompute per-pixel samples in [0,1); we'll reuse the same (x, y)
// sample pattern in all pixels.
void InitSamples() {
    for (int i = 0; i < MAX_SAMPLES_PER_PIXEL; ++i) {
        uint32_t x, y;
        GetXY(i, 24, x, y);
        static const float inv24 = 1.f / (1 << 24);
        xs[i] = (float)x * inv24;
        ys[i] = (float)y * inv24;
    }
}

inline uint32_t ReverseBits(uint32_t n) {
    n = (n << 16) | (n >> 16);
    n = ((n & 0x00ff00ff) << 8) | ((n & 0xff00ff00) >> 8);
    n = ((n & 0x0f0f0f0f) << 4) | ((n & 0xf0f0f0f0) >> 4);
    n = ((n & 0x33333333) << 2) | ((n & 0xcccccccc) >> 2);
    n = ((n & 0x55555555) << 1) | ((n & 0xaaaaaaaa) >> 1);
    return n;
}

inline float VanDerCorput(uint32_t n, uint32_t scramble) {
    // Reverse bits of _n_
    n = ReverseBits(n);
    n ^= scramble;
    return ((n>>8) & 0xffffff) / float(1 << 24);
}


void GetSamples(int xPixel, int yPixel, int samplesPerPixel,
                float xSamples[], float ySamples[],
                float timeSamples[]) {
    assert(samplesPerPixel <= MAX_SAMPLES_PER_PIXEL);
    float invSPP = 1.f / (float)samplesPerPixel;

    for (int i = 0; i < samplesPerPixel; ++i)
      xSamples[i] = xPixel + xs[i];
    for (int i = 0; i < samplesPerPixel; ++i)
      ySamples[i] = yPixel + ys[i];

    float shift = (ReverseBits((xPixel << 8) ^ yPixel) % 317) / 317.f;
    for (int i = 0; i < samplesPerPixel; ++i) {
      float t = (i + shift) * invSPP;
      timeSamples[i] = t;
    }
}


inline float Sobol2(uint32_t n, uint32_t scramble) {
    for (uint32_t v = 1 << 31; n != 0; n >>= 1, v ^= v >> 1)
        if (n & 0x1) scramble ^= v;
    return ((scramble>>8) & 0xffffff) / float(1 << 24);
}


void GetLensSamples(int xPixel, int yPixel, int samplesPerPixel,
                    float uSamples[], float vSamples[]) {
    uint32_t scramble[2] = { ~(((xPixel & 0xf) << 28) | (yPixel>>2)),
                             ~(((yPixel & 0xf) << 28) | (xPixel>>3)) };
    for (int i = 0; i < samplesPerPixel; ++i) {
        uSamples[i] = VanDerCorput(i, scramble[0]);
        vSamples[i] = Sobol2(i, scramble[1]);
    }
}
