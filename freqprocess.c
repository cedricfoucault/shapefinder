/** Filtre dans le domaine spectral avec d�tection de contour et moyenne gaussienne (par convolution avec une gaussienne)
- tab l'image spectrale par FFT d'un signal � 2 dimensions, stock�e dans un tableau � unidimensionel
*/ 
#include "freqprocess.h"
#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
#define UINT8_MAX_VALUE (255)

bwimage_t *correlateLocateShape(bwimage_t *target, bwimage_t *pattern, float variance, float tolerancethreshold, int *npeak, Pixel *maximizer) {
	cimage_t *cTarget = bwimageToCimage(target);
	cimage_t *cPattern = bwimageToCimage(pattern);
	cimage_t *cResult = complexCorrelate(cTarget, cPattern, variance);
	bwimage_t *result = cimageToBwimageMemorizePeaks(cResult, tolerancethreshold, npeak, maximizer);
	freeCimage(cTarget);
	freeCimage(cPattern);
	freeCimage(cResult);
	return result;
}

void trimLocateShape(bwimage_t **corfil, bwimage_t *target, bwimage_t **pattern, float variance, float tolerancethreshold, int *npeak, Pixel *maximizer) {
	cimage_t *cTarget = bwimageToCimage(target);
	cimage_t *cPattern = bwimageToCimage(*pattern);
	cimage_t *cResult;
	complexTrimCor(&cResult, cTarget, &cPattern, variance);
	*pattern = cimageToBwimage(cPattern);
	*corfil = cimageToBwimageMemorizePeaks(cResult, tolerancethreshold, npeak, maximizer);
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
	
	fourn((float*)(*image)->rawdata, (unsigned long*)nn, 2, -1);
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

	fourn((float*)((*image)->rawdata), (unsigned long*)nn, 2, 1);
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
	float variance_dim = (float)height * (float)width * variance; // variance dimensionn�e en fonction de la taille de l'image
	Complex factor = {0.,0.};
	*corfil = createCimage(height, width);
	
	fourn((float*)(target->rawdata), (unsigned long*)nn, 2, -1);
	fourn((float*)((*pattern)->rawdata), (unsigned long*)nn, 2, -1);
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
	
	fourn((float*)((*corfil)->rawdata), (unsigned long*)nn, 2, 1);
	fourn((float*)((*pattern)->rawdata), (unsigned long*)nn, 2, 1);
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
	float variance_dim = (float)height * (float)width * variance; // variance dimensionn�e en fonction de la taille de l'image
	Complex factor = {0.,0.};
	cimage_t *result = createCimage(height, width);
	
	fourn((float*)(target->rawdata), (unsigned long*)nn, 2, -1);
	fourn((float*)(pattern->rawdata), (unsigned long*)nn, 2, -1);
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
	
	fourn((float*)(result->rawdata), (unsigned long*)nn, 2, 1);
	for (i=0; i < height; i++) {
		for (j=0; j < width; j++) {
			result->data[i][j] = result->rawdata[i * width + j];
		}
	}
	return result;
}

void filter(Complex tab[], int height, int width, float variance) {
	int i, j;
	float variance_dim = (float)height * (float)width * variance; // variance dimensionn�e en fonction de la taille de l'image
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

void fourn(float data[], unsigned long nn[], int ndim, int isign)
{

	int idim;
	unsigned long i1,i2,i3,i2rev,i3rev,ip1,ip2,ip3,ifp1,ifp2;
	unsigned long ibit,k1,k2,n,nprev,nrem,ntot;
	float tempi,tempr;
	double theta,wi,wpi,wpr,wr,wtemp;

	data--;
	nn--;

	for (ntot=1,idim=1;idim<=ndim;idim++)
		ntot *= nn[idim];
	nprev=1;
	for (idim=ndim;idim>=1;idim--) {
		n=nn[idim];
		nrem=ntot/(n*nprev);
		ip1=nprev << 1;
		ip2=ip1*n;
		ip3=ip2*nrem;
		i2rev=1;
		for (i2=1;i2<=ip2;i2+=ip1) {
			if (i2 < i2rev) {
				for (i1=i2;i1<=i2+ip1-2;i1+=2) {
					for (i3=i1;i3<=ip3;i3+=ip2) {
						i3rev=i2rev+i3-i2;
						SWAP(data[i3],data[i3rev]);
						SWAP(data[i3+1],data[i3rev+1]);
					}
				}
			}
			ibit=ip2 >> 1;
			while (ibit >= ip1 && i2rev > ibit) {
				i2rev -= ibit;
				ibit >>= 1;
			}
			i2rev += ibit;
		}
		ifp1=ip1;
		while (ifp1 < ip2) {
			ifp2=ifp1 << 1;
			theta=isign*6.28318530717959/(ifp2/ip1);
			wtemp=sin(0.5*theta);
			wpr = -2.0*wtemp*wtemp;
			wpi=sin(theta);
			wr=1.0;
			wi=0.0;
			for (i3=1;i3<=ifp1;i3+=ip1) {
				for (i1=i3;i1<=i3+ip1-2;i1+=2) {
					for (i2=i1;i2<=ip3;i2+=ifp2) {
						k1=i2;
						k2=k1+ifp1;
						tempr=(float)wr*data[k2]-(float)wi*data[k2+1];
						tempi=(float)wr*data[k2+1]+(float)wi*data[k2];
						data[k2]=data[k1]-tempr;
						data[k2+1]=data[k1+1]-tempi;
						data[k1] += tempr;
						data[k1+1] += tempi;
					}
				}
				wr=(wtemp=wr)*wpr-wi*wpi+wr;
				wi=wi*wpr+wtemp*wpi+wi;
			}
			ifp1=ifp2;
		}
		nprev *= n;
	}
}

#undef SWAP

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

