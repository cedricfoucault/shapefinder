/** Filtre dans le domaine spectral avec détection de contour et moyenne gaussienne (par convolution avec une gaussienne)
- tab l'image spectrale par FFT d'un signal à 2 dimensions, stockée dans un tableau à unidimensionel
*/ 
#include "freqprocess.h"
#define UINT8_MAX_VALUE (255)

bwimage_t *correlateLocateShape(bwimage_t *target, bwimage_t *pattern, float variance, int *npeak, Pixel *maximizer) {
	cimage_t *cTarget = bwimageToCimage(target);
	cimage_t *cPattern = bwimageToCimage(pattern);
	cimage_t *cResult = complexCorrelate(cTarget, cPattern, variance);
	bwimage_t *result = cimageToBwimageMemorizePeaks(cResult, npeak, maximizer);
	freeCimage(cTarget);
	freeCimage(cPattern);
	freeCimage(cResult);
	return result;
}

void trimLocateShape(bwimage_t **corfil, bwimage_t *target, bwimage_t **pattern, float variance, int *npeak, Pixel *maximizer) {
	cimage_t *cTarget = bwimageToCimage(target);
	cimage_t *cPattern = bwimageToCimage(*pattern);
	cimage_t *cResult;
	complexTrimCor(&cResult, cTarget, &cPattern, variance);
	*pattern = cimageToBwimage(cPattern);
	*corfil = cimageToBwimageMemorizePeaks(cResult, npeak, maximizer);
	freeCimage(cTarget);
	freeCimage(cPattern);
	freeCimage(cResult);
}

bwimage_t *createPeakImage(int height, int width, int npeak, Pixel peaks[]) {
	bwimage_t *image_peaks;
	int i;
	image_peaks = createBlackBwimage(height, width);
	for (i=0; i < npeak; i++) {
		(image_peaks->data)[peaks[i].y][peaks[i].x] = (uint8)UINT8_MAX_VALUE;
	}
	return image_peaks;
}

void trim(bwimage_t **image) {
	cimage_t *cImage = bwimageToCimage(*image);
	complexTrim(&cImage, (float)0.5);
	*image = cimageToBwimage(cImage);
	freeCimage(cImage);
}

void complexTrim(cimage_t **image, float variance) {
	int height = (*image)->height, width = (*image)->width, i, j;
	int nn[2] = {height, width};
	Complex factor = {0., 0.};
	float variance_dim = variance * (float)height * (float)width;
	
	fourn((float*)(*image)->rawdata, nn, 2, -1);
	for (i=0; i < height/2; i++) {
		for (j=0; j < width/2; j++) {
			factor.re = (float)sqrt(i*i + j*j) * (float)exp(- (float)(i*i + j*j) / (2*variance_dim));
			(*image)->rawdata[i * width + j] = complexMultiply(factor, (*image)->rawdata[i * width + j]);
		}
		for (j = width/2; j < width; j++) {
			factor.re = (float)sqrt(i*i + (j-width)*(j-width)) * (float)exp(- (float)(i*i + j*j) / (2*variance_dim));
			(*image)->rawdata[i * width + j] = complexMultiply(factor, (*image)->rawdata[i * width + j]);
		}
	}
	for (i = height/2; i < height; i++) {
		for (j=0; j < width/2; j++) {
			factor.re = (float)sqrt((i-height)*(i-height) + j*j) * (float)exp(- (float)(i*i + j*j) / (2*variance_dim));
			(*image)->rawdata[i * width + j] = complexMultiply(factor, (*image)->rawdata[i * width + j]);
		}
		for (j = width/2; j < width; j++) {
			factor.re = (float)sqrt((i-height)*(i-height) + (j-width)*(j-width)) * (float)exp(- (float)(i*i + j*j) / (2*variance_dim));
			(*image)->rawdata[i * width + j] = complexMultiply(factor, (*image)->rawdata[i * width + j]);
		}
	}

	fourn((float*)((*image)->rawdata), nn, 2, 1);
	for (i=0; i < height; i++) {
		for (j=0; j < width; j++) {
			(*image)->data[i][j].re = (*image)->rawdata[i * width + j].re = fabs((*image)->rawdata[i * width + j].re);
			(*image)->data[i][j].im = (*image)->rawdata[i * width + j].im;
		}
	}
}

void complexTrimCor(cimage_t **corfil, cimage_t *target, cimage_t **pattern, float variance) {
	int height=target->height, width=target->width, i, j;
	//int height=(*pattern)->height, width=(*pattern)->width, i, j;
	int nn[2] = {height, width};
	float variance_dim = (float)height * (float)width * variance; // variance dimensionnée en fonction de la taille de l'image
	Complex factor = {0.,0.};
	*corfil = createCimage(height, width);
	
	fourn((float*)(target->rawdata), nn, 2, -1);
	fourn((float*)((*pattern)->rawdata), nn, 2, -1);
	for (i=0; i < height/2; i++) {
		for (j=0; j < width/2; j++) {
			(*corfil)->rawdata[i * width + j] = complexMultiply(target->rawdata[i * width + j], complexConjugate((*pattern)->rawdata[i * width + j]));
			factor.re = (float)sqrt(i*i + j*j); //* exp(- (float)(i*i + j*j) / (2*variance_dim));
			(*pattern)->rawdata[i * width + j] = complexMultiply(factor, (*pattern)->rawdata[i * width + j]);
			factor.re = factor.re * (float)sqrt(i*i + j*j) * exp(- (float)(i*i + j*j) / (variance_dim));
			factor.im = 0;
			(*corfil)->rawdata[i * width + j] = complexMultiply(factor, (*corfil)->rawdata[i * width + j]);
		}
		for (j = width/2; j < width; j++) {
			(*corfil)->rawdata[i * width + j] = complexMultiply(target->rawdata[i * width + j], complexConjugate((*pattern)->rawdata[i * width + j]));
			factor.re = (float)sqrt(i*i + (j-width)*(j-width)); //* exp(- (float)(i*i + (j-width)*(j-width)) / (2*variance_dim));
			(*pattern)->rawdata[i * width + j] = complexMultiply(factor, (*pattern)->rawdata[i * width + j]);
			factor.re = factor.re * (float)sqrt(i*i + (j-width)*(j-width)) * exp(- (float)(i*i + (j-width)*(j-width)) / (variance_dim));
			factor.im = 0;
			(*corfil)->rawdata[i * width + j] = complexMultiply(factor, (*corfil)->rawdata[i * width + j]);
		}
	}
	
	for (i = height/2; i < height; i++) {
		for (j=0; j < width/2; j++) {
			(*corfil)->rawdata[i * width + j] = complexMultiply(target->rawdata[i * width + j], complexConjugate((*pattern)->rawdata[i * width + j]));
			factor.re = (float)sqrt((i-height)*(i-height) + j*j); //* exp(- (float)((i-height)*(i-height) + j*j) / (2*variance_dim));
			(*pattern)->rawdata[i * width + j] = complexMultiply(factor, (*pattern)->rawdata[i * width + j]);
			factor.re = factor.re * (float)sqrt((i-height)*(i-height) + j*j) * exp(- (float)((i-height)*(i-height) + j*j) / (variance_dim));
			factor.im = 0;
			(*corfil)->rawdata[i * width + j] = complexMultiply(factor, (*corfil)->rawdata[i * width + j]);
		}
		for (j = width/2; j < width; j++) {
			(*corfil)->rawdata[i * width + j] = complexMultiply(target->rawdata[i * width + j], complexConjugate((*pattern)->rawdata[i * width + j]));
			factor.re = (float)sqrt((i-height)*(i-height) + (j-width)*(j-width)); //* exp(- (float)((i-height)*(i-height) + (j-width)*(j-width)) / (2*variance_dim));
			(*pattern)->rawdata[i * width + j] = complexMultiply(factor, (*pattern)->rawdata[i * width + j]);
			factor.re = factor.re * (float)sqrt((i-height)*(i-height) + (j-width)*(j-width)) * exp(- (float)((i-height)*(i-height) + (j-width)*(j-width)) / (variance_dim));
			factor.im = 0;
			(*corfil)->rawdata[i * width + j] = complexMultiply(factor, (*corfil)->rawdata[i * width + j]);
		}
	}
	
	fourn((float*)((*corfil)->rawdata), nn, 2, 1);
	fourn((float*)((*pattern)->rawdata), nn, 2, 1);
	for (i=0; i < height; i++) {
		for (j=0; j < width; j++) {
			(*pattern)->data[i][j].re = (*pattern)->rawdata[i * width + j].re = fabs((*pattern)->rawdata[i * width + j].re);
			(*corfil)->data[i][j] = (*corfil)->rawdata[i * width + j];
			(*pattern)->data[i][j].im = (*pattern)->rawdata[i * width + j].im;
		}
	}
}

cimage_t *complexCorrelate(cimage_t *target, cimage_t *pattern, float variance) {
	int height=target->height, width=target->width, i, j;
	int nn[2] = {height, width};
	float variance_dim = (float)height * (float)width * variance; // variance dimensionnée en fonction de la taille de l'image
	Complex factor = {0.,0.};
	cimage_t *result = createCimage(height, width);
	
	fourn((float*)(target->rawdata), nn, 2, -1);
	fourn((float*)(pattern->rawdata), nn, 2, -1);
	for (i=0; i < height/2; i++) {
		for (j=0; j < width/2; j++) {
			result->rawdata[i * width + j] = complexMultiply(target->rawdata[i * width + j], complexConjugate(pattern->rawdata[i * width + j]));
			factor.re = (float)(i*i + j*j) * exp(- (float)(i*i + j*j) / variance_dim);
			result->rawdata[i*width + j] = complexMultiply(factor, result->rawdata[i * width + j]);
		}
		for (j = width/2; j < width; j++) {
			result->rawdata[i * width + j] = complexMultiply(target->rawdata[i * width + j], complexConjugate(pattern->rawdata[i * width + j]));
			factor.re = (float)(i*i + (j-width)*(j-width)) * exp(- (float)(i*i + (j-width)*(j-width)) / variance_dim);
			result->rawdata[i*width + j] = complexMultiply(factor, result->rawdata[i * width + j]);
		}
	}
	
	for (i = height/2; i < height; i++) {
		for (j=0; j < width/2; j++) {
			result->rawdata[i * width + j] = complexMultiply(target->rawdata[i * width + j], complexConjugate(pattern->rawdata[i * width + j]));
			factor.re = (float)((i-height)*(i-height) + j*j) * exp(- (float)((i-height)*(i-height) + j*j) / variance_dim);
			result->rawdata[i*width + j] = complexMultiply(factor, result->rawdata[i * width + j]);
		}
		for (j = width/2; j < width; j++) {
			result->rawdata[i * width + j] = complexMultiply(target->rawdata[i * width + j], complexConjugate(pattern->rawdata[i * width + j]));
			factor.re = (float)((i-height)*(i-height) + (j-width)*(j-width)) * exp(- (float)((i-height)*(i-height) + (j-width)*(j-width)) / variance_dim);
			result->rawdata[i*width + j] = complexMultiply(factor, result->rawdata[i * width + j]);
		}
	}
	
	fourn((float*)(result->rawdata), nn, 2, 1);
	for (i=0; i < height; i++) {
		for (j=0; j < width; j++) {
			result->data[i][j] = result->rawdata[i * width + j];
		}
	}
	return result;
}

void filter(Complex tab[], int height, int width, float variance) {
	int i, j;
	float variance_dim = (float)height * (float)width * variance; // variance dimensionnée en fonction de la taille de l'image
	Complex factor = {0.,0.};
	
	for (i=0; i < height/2; i++) {
		for (j=0; j < width/2; j++) {
			factor.re = (float)(i*i + j*j) * exp(- (float)(i*i + j*j) / variance_dim);
			tab[i*width + j] = complexMultiply(factor, tab[i*width + j]);
		}
		for (j = width/2; j < width; j++) {
			factor.re = (float)(i*i + (j-width)*(j-width)) * exp(- (float)(i*i + (j-width)*(j-width)) / variance_dim);
			tab[i*width + j] = complexMultiply(factor, tab[i*width + j]);
		}
	}
	
	for (i = height/2; i < height; i++) {
		for (j=0; j < width/2; j++) {
			factor.re = (float)((i-height)*(i-height) + j*j) * exp(- (float)((i-height)*(i-height) + j*j) / variance_dim);
			tab[i*width + j] = complexMultiply(factor, tab[i*width + j]);
		}
		for (j = width/2; j < width; j++) {
			factor.re = (float)((i-height)*(i-height) + (j-width)*(j-width)) * exp(- (float)((i-height)*(i-height) + (j-width)*(j-width)) / variance_dim);
			tab[i*width + j] = complexMultiply(factor, tab[i*width + j]);
		}
	}
}

// High-pass filter:

/*
		int nn[2] = {256, 256};
		cimage_t *image = (cimage_t *) malloc(sizeof(cimage_t));
		bwimage_t *real_image;
		image->height = image->width = 256;
		image->rawdata = (Complex *) malloc(sizeof(Complex)*256*256);
		image->data = (Complex **) malloc(sizeof(Complex *)*256);
		for (k=0; k < 256/2; k++) {
			image->data[k] = (Complex *) malloc(sizeof(Complex)*256);
			for (i=0; i < 256/2; i++) {
				image->rawdata[k*256 + i].re = (float)sqrt(k*k + i*i);
			}
			for(i=256/2; i < 256; i ++) {
				image->rawdata[k*256 + i].re = (float)sqrt(k*k + (i-256)*(i-256));
			}
		}
		for (k=256/2; k < 256; k++){
			image->data[k] = (Complex *) malloc(sizeof(Complex)*256);
			for (i=0; i < 256/2; i++) {
				image->rawdata[k*256 + i].re = (float)sqrt((k-256)*(k-256) + i*i);
			}
			for(i=256/2; i < 256; i ++) {
				image->rawdata[k*256 + i].re = (float)sqrt((k-256)*(k-256) + (i-256)*(i-256));
			}
		}

		for (k=0; k < 256; k++) {
			for (i=0; i < 256; i++) {
				image->data[k][i] = image->rawdata[k*256+i];
			}
		}
		real_image = cimageToBwimage(image);
		if(EEA_OK != (retval=EEADumpImage("samples/filter_highpass.tif", real_image))) break;

*/