#include "cvwrap.h"

void iplImageToBwimage(bwimage_t **target, IplImage *src) {
    int i, j, height, width;
    *target = (bwimage_t *) malloc (sizeof(bwimage_t));
    height = (*target)->height = src->height;
    width = (*target)->width = src->width;
    (*target)->rawdata = (uint8 *) malloc(height * width * sizeof(uint8));
    (*target)->data = (uint8 **) malloc(height * sizeof(uint8 *));
    for (i = 0; i < height; i++) {
        (*target)->data[i] = (uint8 *) malloc(width * sizeof(uint8));
        for (j = 0; j < width; j++) {
            (*target)->data[i][j] = (*target)->rawdata[i * width + j] = src->imageData[i * width + j];
        }
    }
}
