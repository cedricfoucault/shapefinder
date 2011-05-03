#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tifwrap.h"
#include "freqprocess.h"

#define MAX_PATH 256

// fgetstr() - mimics behavior of fgets(), but removes new-line
// character at end of line if it exists
char *fgetstr(char *string, int n, FILE *stream)
{
	char *result;
	result = fgets(string, n, stream);
	if(!result)
		return(result);

	if(string[strlen(string) - 1] == '\n')
		string[strlen(string) - 1] = '\0';

	return(string);
}

int main()
{
	FILE *currentFile = NULL;
	char *target_path = (char*) malloc(sizeof(char) * MAX_PATH);
	char *pattern_path = (char*) malloc(sizeof(char) * MAX_PATH);
	char *shapes_path = (char*) malloc(sizeof(char) * MAX_PATH);
	char *peaks_path = (char*) malloc(sizeof(char) * MAX_PATH);
	char *corfil_path = (char*) malloc(sizeof(char) * MAX_PATH);
	float tolerancethreshold = (float)0.;
	bwimage_t *image_target, *image_pattern, *image_corfil, *image_peaks, *image_shapes;
	int npeak, i, j, k;
	Pixel *peaks;
	
	error_e retval=EEA_OK;
	
	image_target=EEACreateImage();
	image_pattern=EEACreateImage();
	
	do
	{
		// file containing the paths to the images
		currentFile = fopen("paths.txt", "r"); 
		if (currentFile!=NULL) {
			fgetstr(target_path, MAX_PATH, currentFile);
			fgetstr(pattern_path, MAX_PATH, currentFile);
			fgetstr(shapes_path, MAX_PATH, currentFile);
			fgetstr(peaks_path, MAX_PATH, currentFile);
			fgetstr(corfil_path, MAX_PATH, currentFile);
		} else {
			printf("paths.txt cannot be opened");
			getchar();
			exit(EXIT_FAILURE);
		}
		fclose(currentFile);
		// file containing the tolerance threshold
		currentFile = fopen("tolerancethreshold.txt", "r");
		if (currentFile != NULL) {
		if (fscanf(currentFile, "%f", &tolerancethreshold) == EOF
			|| tolerancethreshold == (float)0.) {
			tolerancethreshold = (float)0.7; // default value if no threshold is specified
		}
		} else {
			tolerancethreshold = (float)0.7;
		}

		retval = EEALoadImage(target_path, image_target);
		if(retval != EEA_OK) break;
		retval = EEALoadImage(pattern_path, image_pattern);
		if(retval != EEA_OK) break;
		free(target_path);
		free(pattern_path);
		if (image_target->height >= image_target->width) {
			peaks = (Pixel *) malloc(image_target->height * sizeof(Pixel *));
		} else {
			peaks = (Pixel *) malloc(image_target->width * sizeof(Pixel *));
		}

		trimLocateShape(&image_corfil, image_target, &image_pattern, 0.08, tolerancethreshold, &npeak, peaks);
		image_peaks = createPeakImage(image_corfil->height, image_corfil->width, npeak, peaks);
		image_shapes = createBlackBwimage(image_target->height, image_target->width);
		for (k=0; k < npeak; k++) {
			for (i=0; i < (int)image_target->height; i++) {
				for (j=0; j < (int)image_target->width; j++) {
					if (image_pattern->data[i][j] != 0) {
						image_shapes->data[(i + peaks[k].y) % image_target->height][(j + peaks[k].x) % image_target->width] = image_pattern->data[i][j];
					}
				}
			}
		}
		if(EEA_OK != (retval=EEADumpImage(shapes_path, image_shapes))) break;
		if(EEA_OK != (retval=EEADumpImage(peaks_path, image_peaks))) break;
		if(EEA_OK != (retval=EEADumpImage(corfil_path, image_corfil))) break;
	}
	

	/*do {
		retval = EEALoadImage("samples/pattern_triangle.tif", image_target);
		trim(&image_target);
		if(EEA_OK != (retval=EEADumpImage("samples/pattern_triangle_trimmed.tif", image_target))) break;
	}*/
	
	while(0);
	
	switch(retval)
	{
	case EEA_OK:
		break;
	case EEA_ENOFILE:
		fprintf(stderr, "Cannot open file\n");
		break;
	case EEA_EBADBPS:
		fprintf(stderr, "Number of bits per sample must be equal to 8\n");
		break;
	case EEA_EBADPHOTOMETRIC:
		fprintf(stderr, "Not a colormap image\n");
		break;
	case EEA_ENOCOLORMAP:
		fprintf(stderr, "Image does not have a colormap\n");
		break;
	case EEA_ENOTGRAY:
		fprintf(stderr, "At least one entry in the colormap is not gray\n");
	case EEA_ENOMEM:
		fprintf(stderr, "Cannot allocate memory\n");
		break;
	default:
		;/* Can't happen */
	}
	free(shapes_path);
	free(peaks_path);
	free(corfil_path);
	free(peaks);
	EEAFreeImage(image_target);
	EEAFreeImage(image_pattern);
	EEAFreeImage(image_corfil);
	EEAFreeImage(image_peaks);
	
	return 0;
}
