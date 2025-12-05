#ifndef CACTUS_H
#define CACTUS_H


typedef struct CactusBranchConfig
{
    float length;
    float radiusBase;
    float radiusTip;
    float bend;
    float forkT;
    float forkAngleDeg;
    float forkScale;
    float slices;
} CactusBranchConfig;

// Draw a single cactus branch using the provided config.
// The caller can position/scale with the current matrix before calling.
void draw_cactus(const CactusBranchConfig *config);


#endif // CACTUS_H
