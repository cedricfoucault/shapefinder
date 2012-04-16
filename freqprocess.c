#include "freqprocess.h"
#define UINT8_MAX_VALUE (255)

bwimage_t *correlateLocateShape(bwimage_t *target, bwimage_t *pattern,
                                float variance, float tolerancethreshold,
                                int *npeak, Pixel *maximizer) {
    cimage_t *cTarget = bwimageToCimage(target);
    cimage_t *cPattern = bwimageToCimage(pattern);
    cimage_t *cResult = complexCorrelate(cTarget, cPattern, variance);
    bwimage_t *result = cimageToBwimageMemorizePeaks(cResult, tolerancethreshold, npeak, maximizer);
    freeCimage(cTarget);
    freeCimage(cPattern);
    freeCimage(cResult);
    return result;
}

void trimLocateShape(bwimage_t **corfil, bwimage_t *target, bwimage_t **pattern,
                     float variance, float tolerancethreshold, int *npeak,
                     Pixel *maximizer) {
    cimage_t *cTarget = bwimageToCimage(target);
    cimage_t *cPattern = bwimageToCimage(*pattern);
    cimage_t *cResult;
    complexTrimCor(&cResult, cTarget, &cPattern, variance);
    *pattern = cimageToBwimage(cPattern);
    *corfil = cimageToBwimageMemorizePeaks(cResult, tolerancethreshold,
                                                npeak, maximizer);
    freeCimage(cTarget);
    freeCimage(cPattern);
    freeCimage(cResult);
}

void locateShape(bwimage_t *target, bwimage_t *pattern,
                 float variance, Pixel *peak) {
    cimage_t *cTarget = bwimageToCimage(target);
    cimage_t *cPattern = bwimageToCimage(pattern);
    cimage_t *cResult = complexCorrelate(cTarget, cPattern, variance);
    findPeak(cResult, peak);
    freeCimage(cTarget);
    freeCimage(cPattern);
    freeCimage(cResult);
}

void findPeak(cimage_t *correlation, Pixel *peak) {
    	int i, j;
        int height = correlation->height, width = correlation->width;
        Complex *rawdata = correlation->rawdata;
        float max = rawdata[0].re;
        int x = 0, y = 0;
	for (i=0; i < height; i++) {
		for (j=0; j < width; j++) {
			if (rawdata[i * width + j].re > max) {
                            max = rawdata[i * width + j].re;
                            x = j;
                            y = i;
			}
		}
	}
        peak->x = x;
        peak->y = y;
}

void createPeakImage(bwimage_t **image_peaks, int height, int width,
                        Pixel peaks[], int npeak) {
    int i;
    *image_peaks = createBlackBwimage(height, width);
    for (i = 0; i < npeak; i++) {
        ((*image_peaks)->data)[peaks[i].y][peaks[i].x] = (uint8) UINT8_MAX_VALUE;
    }
}

void createCenterImage(bwimage_t **image_center, int height, int width,
                        Pixel peak, Pixel center) {
    int i, j;
    *image_center = createWhiteBwimage(height, width);
    for (i = 0; i <= 1; i++) {
        for (j = 0; j <= 1; j++) {
            (*image_center)->data
                    [(peak.y + center.y) % height + i]
                    [(peak.x + center.x) % width + j] 
                                                        = 0;
        }
    }
}

void addPosition(bwimage_t *image_center, Pixel peak, Pixel center) {
    int height = image_center->height, width = image_center->width;
    int i, j;
    for (i = 0; i <= 1; i++) {
        for (j = 0; j <= 1; j++) {
            image_center->data[(peak.y + center.y + i) % height]
                              [(peak.x + center.x + j) % width] = 0;
        }
    }
}

void fillWithShapes(bwimage_t **image_shapes, bwimage_t *image_pattern,
                        int height, int width, Pixel peaks[], int npeak) {
    int i, j, k;
    *image_shapes = createBlackBwimage(height, width);
    for (k = 0; k < npeak; k++) {
        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                if (image_pattern->data[i][j] != 0) {
                    (*image_shapes)->data[(i + peaks[k].y) % height]
                                         [(j + peaks[k].x) % width] 
                                                = image_pattern->data[i][j];
                }
            }
        }
    }
}

void findCenter(Pixel *center, bwimage_t *pattern) {
    // Compute the center of a single object in an image
    int x = 0, y = 0, i, j, n = 0;
    for (i = 0; i < pattern->height; i++) {
        for (j = 0; j < pattern->width; j++) {
            if (pattern->data[i][j] > 0) {
                x += j;
                y += i;
                n++;
            }
        }
    }
    x = (int) ((float) x / (float) n + 0.5); // average x position
    y = (int) ((float) y / (float) n + 0.5); // average y position

    center->x = x;
    center->y = y;
}

void trim(bwimage_t **image, float variance) {
    cimage_t *cImage = bwimageToCimage(*image);
    complexTrim(&cImage, variance);
    EEAFreeImage(*image);
    *image = cimageToBwimage(cImage);
    freeCimage(cImage);
}

void complexTrim(cimage_t **image, float variance) {
    int height = (*image)->height, width = (*image)->width, i, j;
    int nn[2] = {height, width};
    Complex factor = {0., 0.};
    float variance_dim = variance * (float) height * (float) width;

    fourn((float*) (*image)->rawdata, (unsigned long*) nn, 2, -1);
    for (i = 0; i < height / 2; i++) {
        for (j = 0; j < width / 2; j++) {
            factor.re = (float) sqrt(i * i + j * j) *
                        (float) exp(- (float)(i*i + j*j) / (2 * variance_dim));
            (*image)->rawdata[i * width + j] =
                    complexMultiply(factor, (*image)->rawdata[i * width + j]);
        }
        for (j = width / 2; j < width; j++) {
            factor.re = (float) sqrt(i * i + (j - width)*(j - width)) *
                    (float) exp(-(float) (i * i + j * j) / (2 * variance_dim));
            (*image)->rawdata[i * width + j] =
                    complexMultiply(factor, (*image)->rawdata[i * width + j]);
        }
    }
    for (i = height / 2; i < height; i++) {
        for (j = 0; j < width / 2; j++) {
            factor.re = (float) sqrt((i - height)*(i - height) + j * j) *
                    (float) exp(-(float) (i * i + j * j) / (2 * variance_dim));
            (*image)->rawdata[i * width + j] =
                    complexMultiply(factor, (*image)->rawdata[i * width + j]);
        }
        for (j = width / 2; j < width; j++) {
            factor.re = 
            (float) sqrt((i - height)*(i - height) + (j - width)*(j - width)) *
                    (float) exp(-(float) (i * i + j * j) / (2 * variance_dim));
            (*image)->rawdata[i * width + j] =
                    complexMultiply(factor, (*image)->rawdata[i * width + j]);
        }
    }

    fourn((float*) ((*image)->rawdata), (unsigned long*) nn, 2, 1);
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            (*image)->data[i][j].re = (*image)->rawdata[i * width + j].re
                                    = fabs((*image)->rawdata[i * width + j].re);
            (*image)->data[i][j].im = (*image)->rawdata[i * width + j].im;
        }
    }
}

void complexTrimCor(cimage_t **corfil, cimage_t *target,
                        cimage_t **pattern, float variance) {
    int height = target->height, width = target->width, i, j;
    int nn[2] = {height, width};
    float variance_dim = (float) height * (float) width * variance;
    Complex factor = {0., 0.};
    *corfil = createCimage(height, width);

    fourn((float*) (target->rawdata), (unsigned long*) nn, 2, -1);
    fourn((float*) ((*pattern)->rawdata), (unsigned long*) nn, 2, -1);
    for (i = 0; i < height / 2; i++) {
        for (j = 0; j < width / 2; j++) {
            (*corfil)->rawdata[i * width + j] =
                    complexMultiply(target->rawdata[i * width + j],
                          complexConjugate((*pattern)->rawdata[i * width + j]));
            factor.re = (float) sqrt(i * i + j * j);
            (*pattern)->rawdata[i * width + j] =
                    complexMultiply(factor, (*pattern)->rawdata[i * width + j]);
            factor.re = factor.re * (float) sqrt(i * i + j * j) *
                        exp(-(float) (i * i + j * j) / (variance_dim));
            factor.im = 0;
            (*corfil)->rawdata[i * width + j] =
                    complexMultiply(factor,(*corfil)->rawdata[i * width + j]);
        }
        for (j = width / 2; j < width; j++) {
            (*corfil)->rawdata[i * width + j] =
                    complexMultiply(target->rawdata[i * width + j],
                          complexConjugate((*pattern)->rawdata[i * width + j]));
            factor.re = (float) sqrt(i * i + (j - width)*(j - width));
            (*pattern)->rawdata[i * width + j] =
                    complexMultiply(factor, (*pattern)->rawdata[i * width + j]);
            factor.re = factor.re *
                   (float) sqrt(i * i + (j - width)*(j - width)) *
                   exp(-(float) (i * i + (j - width)*(j - width)) / (variance_dim));
            factor.im = 0;
            (*corfil)->rawdata[i * width + j] =
                    complexMultiply(factor, (*corfil)->rawdata[i * width + j]);
        }
    }

    for (i = height / 2; i < height; i++) {
        for (j = 0; j < width / 2; j++) {
            (*corfil)->rawdata[i * width + j] = 
                    complexMultiply(target->rawdata[i * width + j],
                          complexConjugate((*pattern)->rawdata[i * width + j]));
            factor.re = (float) sqrt((i - height)*(i - height) + j * j);
            (*pattern)->rawdata[i * width + j] =
                    complexMultiply(factor, (*pattern)->rawdata[i * width + j]);
            factor.re = factor.re *
                    (float) sqrt((i - height)*(i - height) + j * j) *
                    exp(-(float) ((i - height)*(i - height) + j * j) /
                    (variance_dim));
            factor.im = 0;
            (*corfil)->rawdata[i * width + j] =
                    complexMultiply(factor, (*corfil)->rawdata[i * width + j]);
        }
        for (j = width / 2; j < width; j++) {
            (*corfil)->rawdata[i * width + j] =
                    complexMultiply(target->rawdata[i * width + j],
                    complexConjugate((*pattern)->rawdata[i * width + j]));
            factor.re = (float) sqrt((i - height)*(i - height) + (j - width)*(j - width));
            (*pattern)->rawdata[i * width + j] =
                    complexMultiply(factor, (*pattern)->rawdata[i * width + j]);
            factor.re = factor.re *
                    sqrt((i - height)*(i - height) + (j - width)*(j - width))
                    * exp(-(float) ((i-height)*(i-height)+(j-width)*(j-width)) /
                    (variance_dim));
            factor.im = 0;
            (*corfil)->rawdata[i * width + j] =
                    complexMultiply(factor, (*corfil)->rawdata[i * width + j]);
        }
    }

    fourn((float*) ((*corfil)->rawdata), (unsigned long*) nn, 2, 1);
    fourn((float*) ((*pattern)->rawdata), (unsigned long*) nn, 2, 1);
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            (*pattern)->data[i][j].re = (*pattern)->rawdata[i * width + j].re
                                = fabs((*pattern)->rawdata[i * width + j].re);
            (*corfil)->data[i][j] = (*corfil)->rawdata[i * width + j];
            (*pattern)->data[i][j].im = (*pattern)->rawdata[i * width + j].im;
        }
    }
}

cimage_t *complexCorrelate(cimage_t *target, cimage_t *pattern, float variance){
    int height = target->height, width = target->width, i, j;
    int nn[2] = {height, width};
    float variance_dim = (float) height * (float) width * variance;
    Complex factor = {0., 0.};
    cimage_t *result = createCimage(height, width);

    fourn((float*) (target->rawdata), (unsigned long*) nn, 2, -1);
    fourn((float*) (pattern->rawdata), (unsigned long*) nn, 2, -1);
    for (i = 0; i < height / 2; i++) {
        for (j = 0; j < width / 2; j++) {
            result->rawdata[i * width + j] =
                    complexMultiply(target->rawdata[i * width + j],
                    complexConjugate(pattern->rawdata[i * width + j]));
            factor.re = (float) (i * i + j * j) * exp(-(float) (i * i + j * j) /
                                                        variance_dim);
            result->rawdata[i * width + j] =
                    complexMultiply(factor, result->rawdata[i * width + j]);
        }
        for (j = width / 2; j < width; j++) {
            result->rawdata[i * width + j] =
                    complexMultiply(target->rawdata[i * width + j],
                    complexConjugate(pattern->rawdata[i * width + j]));
            factor.re = (float) (i * i + (j - width)*(j - width)) *
                    exp(-(float) (i * i + (j - width)*(j - width)) /
                                                        variance_dim);
            result->rawdata[i * width + j] =
                    complexMultiply(factor, result->rawdata[i * width + j]);
        }
    }

    for (i = height / 2; i < height; i++) {
        for (j = 0; j < width / 2; j++) {
            result->rawdata[i * width + j] =
                    complexMultiply(target->rawdata[i * width + j],
                    complexConjugate(pattern->rawdata[i * width + j]));
            factor.re = (float) ((i - height)*(i - height) + j * j) *
                    exp(-(float) ((i - height)*(i - height) + j * j) /
                                                                variance_dim);
            result->rawdata[i * width + j] =
                    complexMultiply(factor, result->rawdata[i * width + j]);
        }
        for (j = width / 2; j < width; j++) {
            result->rawdata[i * width + j] =
                    complexMultiply(target->rawdata[i * width + j],
                    complexConjugate(pattern->rawdata[i * width + j]));
            factor.re = ((i - height)*(i - height) + (j - width)*(j - width)) *
                    exp(- ((i - height)*(i - height) + (j - width)*(j - width))/
                    variance_dim);
            result->rawdata[i * width + j] = 
                    complexMultiply(factor, result->rawdata[i * width + j]);
        }
    }

    fourn((float*) (result->rawdata), (unsigned long*) nn, 2, 1);
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            result->data[i][j] = result->rawdata[i * width + j];
        }
    }
    return result;
}

void filter(Complex tab[], int height, int width, float variance) {
    int i, j;
    float variance_dim = (float) height * (float) width * variance;
    Complex factor = {0., 0.};

    for (i = 0; i < height / 2; i++) {
        for (j = 0; j < width / 2; j++) {
            factor.re = (float) (i * i + j * j) * exp(-(float) (i * i + j * j) /
                    variance_dim);
            tab[i * width + j] = complexMultiply(factor, tab[i * width + j]);
        }
        for (j = width / 2; j < width; j++) {
            factor.re = (float) (i * i + (j - width)*(j - width)) *
                 exp(-(float) (i * i + (j - width)*(j - width)) / variance_dim);
            tab[i * width + j] = complexMultiply(factor, tab[i * width + j]);
        }
    }

    for (i = height / 2; i < height; i++) {
        for (j = 0; j < width / 2; j++) {
            factor.re = (float) ((i - height)*(i - height) + j * j) *
                    exp(-(float) ((i - height)*(i - height) + j * j) /
                                                                variance_dim);
            tab[i * width + j] = complexMultiply(factor, tab[i * width + j]);
        }
        for (j = width / 2; j < width; j++) {
            factor.re = ((i - height)*(i - height) + (j - width)*(j - width)) *
                    exp(-((i - height)*(i - height) + (j - width)*(j - width)) /
                                                                variance_dim);
            tab[i * width + j] = complexMultiply(factor, tab[i * width + j]);
        }
    }
}