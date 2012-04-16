#ifndef FREQPROCESS_H
#define FREQPROCESS_H
#include "wrap.h"
#include "math.h"

void trim(bwimage_t **image, float variance);
void locateShape(bwimage_t *target, bwimage_t *pattern,
                 float variance, Pixel *peak);
void complexTrim(cimage_t **image, float variance);
void trimLocateShape(bwimage_t **corfil, bwimage_t *target,
                        bwimage_t **pattern, float variance,
                        float tolerancethreshold, int *npeak, Pixel *maximizer);
void complexTrimCor(cimage_t **corfil, cimage_t *target,
                        cimage_t **pattern, float variance);
bwimage_t *correlateLocateShape(bwimage_t *target, bwimage_t *pattern,
                                float variance, float tolerancethreshold,
                                int *npeak, Pixel *maximizer);
bwimage_t *correlate(bwimage_t *target, bwimage_t *pattern, float variance);
void createPeakImage(bwimage_t **image_peaks, int height,
                        int width, Pixel peaks[], int npeak);
void fillWithShapes(bwimage_t **image_shapes, bwimage_t *image_pattern,
                        int height, int width, Pixel peaks[], int npeak);
void createCenterImage(bwimage_t **image_center, int height,
                        int width, Pixel peak, Pixel center);
void addPosition(bwimage_t *image_center, Pixel peak, Pixel center);
void findCenter(Pixel *center, bwimage_t *pattern);
void findPeak(cimage_t *correlation, Pixel *peak);
cimage_t *complexCorrelate(cimage_t *target, cimage_t *pattern, float variance);
void filter(Complex tab[], int height, int width, float variance);

#endif
