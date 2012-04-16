#include "findShape.h"

int findShape() {
    FILE *currentFile = NULL;
    char *target_path = (char*) malloc(sizeof (char) * MAX_PATH);
    char *pattern_path = (char*) malloc(sizeof (char) * MAX_PATH);
    char *shapes_path = (char*) malloc(sizeof (char) * MAX_PATH);
    char *peaks_path = (char*) malloc(sizeof (char) * MAX_PATH);
    char *corfil_path = (char*) malloc(sizeof (char) * MAX_PATH);
    float tolerancethreshold = (float) 0.;
    bwimage_t *image_target, *image_pattern, *image_corfil, *image_peaks,
                *image_shapes;
    int npeak;
    Pixel *peaks;
    error_e retval = EEA_OK;
    image_target = EEACreateImage();
    image_pattern = EEACreateImage();
    
    /***** MAIN PROCEDURE *****/
    do {
        /*** Find the paths to the images that need to be processed ***/
        currentFile = fopen("paths.txt", "r"); // file containing the paths
                                              //  to the images
        if (currentFile != NULL) {
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
        
    /*** Find the tolerance threshold to be used (0.7 is the default value) ***/
        currentFile = fopen("tolerancethreshold.txt", "r");
        if (currentFile != NULL) {
            if (fscanf(currentFile, "%f", &tolerancethreshold) == EOF
                    || tolerancethreshold == (float) 0.) {
                tolerancethreshold = (float) 0.7; // default value
            }
        } else {
            tolerancethreshold = (float) 0.7;
        }
        fclose(currentFile);
        
        /*** Load the input images ***/
        retval = EEALoadImage(target_path, image_target);
        if (retval != EEA_OK) break;
        retval = EEALoadImage(pattern_path, image_pattern);
        if (retval != EEA_OK) break;
        free(target_path);
        free(pattern_path);
        if (image_target->height >= image_target->width) {
            peaks = (Pixel *) malloc(image_target->height * sizeof (Pixel *));
        } else {
            peaks = (Pixel *) malloc(image_target->width * sizeof (Pixel *));
        }

        /*** Process the images ***/
        trimLocateShape(&image_corfil, image_target, &image_pattern, 0.08,
                                tolerancethreshold, &npeak, peaks);
        createPeakImage(&image_peaks, image_corfil->height,
                                image_corfil->width, peaks, npeak);
        fillWithShapes(&image_shapes, image_pattern, image_target->height,
                                image_target->width, peaks, npeak);
        
        /*** Write the output images at the specified path ***/
        if (EEA_OK != (retval = EEADumpImage(shapes_path, image_shapes))) break;
        if (EEA_OK != (retval = EEADumpImage(peaks_path, image_peaks))) break;
        if (EEA_OK != (retval = EEADumpImage(corfil_path, image_corfil))) break;

    } while (0);
    
    /***** FREE HEAP MEMORY *****/
    free(shapes_path);
    free(peaks_path);
    free(corfil_path);
    free(peaks);
    EEAFreeImage(image_target);
    EEAFreeImage(image_pattern);
    EEAFreeImage(image_corfil);
    EEAFreeImage(image_peaks);
    EEAFreeImage(image_shapes);
    
    /***** ERROR HANDLING *****/
    switch (retval) {
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
            ; /* Can't happen */
    }

    return 0;
}
