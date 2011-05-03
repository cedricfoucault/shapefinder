#ifndef FREQPROCESS
#define FREQPROCESS
#include "complex.h"
#include "math.h"

void trim(bwimage_t **image);
void complexTrim(cimage_t **image, float variance);
void trimLocateShape(bwimage_t **corfil, bwimage_t *target, bwimage_t **pattern, float variance, float tolerancethreshold, int *npeak, Pixel *maximizer);
void complexTrimCor(cimage_t **corfil, cimage_t *target, cimage_t **pattern, float variance);
bwimage_t *correlateLocateShape(bwimage_t *target, bwimage_t *pattern, float variance, float tolerancethreshold, int *npeak, Pixel *maximizer);
bwimage_t *correlate(bwimage_t *target, bwimage_t *pattern, float variance);
bwimage_t *createPeakImage(int height, int width, int npeak, Pixel peaks[]);
cimage_t *complexCorrelate(cimage_t *target, cimage_t *pattern, float variance);
void filter(Complex tab[], int height, int width, float variance);
void fourn(float data[], unsigned long nn[], int ndim, int isign);

#endif
