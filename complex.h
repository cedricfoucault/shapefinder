#ifndef COMPLEX
#define COMPLEX
#include "tiffio.h"
#include "tifwrap.h"

//typedef struct Complex Complex;
typedef struct  {
	float re; /* partie reelle */
	float im; /* partie imaginaire */
} Complex;

typedef struct {
	uint32 width;
	uint32 height;
	Complex **data;
	Complex *rawdata;
} cimage_t;

typedef struct {
	int x;
	int y;
} Pixel;

bwimage_t *cimageToBwimageMemorizePeaks(cimage_t *complexImage, float tolerancethreshold, int *npeak, Pixel *maximizer);
cimage_t *bwimageToCimage(bwimage_t *image);
bwimage_t *cimageToBwimage(cimage_t *complexImage);

cimage_t *createCimage(int height, int width);
bwimage_t *createBlackBwimage(int height, int width);
bwimage_t *createWhiteBwimage(int height, int width);

Complex complexMultiply(Complex c1, Complex c2);
Complex complexConjugate(Complex c);

Complex uintToComplex(uint8);
Complex *tab1UintToComplex(uint8 *tab1Uint, int size);
uint8 *tab1ComplexToUint(Complex *tab1Complex, int size);
Complex **tab2UintToComplex(uint8 **tab2Uint, int height, int width);
uint8 **tab2ComplexToUint(Complex **tab2Complex, int height, int width);
void printTab1Complex(Complex* tabComplex, int size);

float *tab1ComplexToFloat(Complex *tab1Complex, int size);
float **tab2ComplexToFloat(Complex **tab2Complex, int height, int size);
void maxMinTab1Float(float *tab1Float, int size, float *max, float *min);
void maxMinTab2Float(float **tab2Float, int height, int width, float *max, float *min);
void maxMinTab1Complex(Complex *tab1Complex, int size, float *max, float *min);
void maxMinTab2Complex(Complex **tab2Complex, int height, int width, float *max, float *min);

void locatePeaks(Complex **tab2Complex, int height, int width, float max, float tolerancethreshold, int *npeak, Pixel *maximizer);

void freeCimage(cimage_t *complexImage);
void freeBwimage(bwimage_t *image);
void freeTab2Complex(Complex **tab2Complex, int height);
void freeTab2Float(float **tab2Float, int height);
void freeTab2Uint(uint8 **tab2Uint, int height);
void freeTab1Complex(Complex* tab1Complex);
void freeTab1Float(float *tab1Float);
void freeTab1Uint(uint8 *tab1Uint);

#endif
