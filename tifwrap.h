#ifndef __EEA_TIFWRAP_H__
#define __EEA_TIFWRAP_H__

#include "tiffio.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum{
  EEA_OK, EEA_ENOFILE, EEA_EBADBPS, EEA_EBADPHOTOMETRIC, EEA_ENOCOLORMAP, 
    EEA_ENOTGRAY, EEA_ENOMEM
} error_e;


typedef struct{
  uint32 width;
  uint32 height;
  uint8 **data; /* Access intensities as image.data[row][col] */
  uint8 *rawdata;
} bwimage_t;


/* Creates the structure on the heap and initialize it to the defaults */
bwimage_t *EEACreateImage();

/* Loads image from the open tiff file. Data are allocated on the heap. */
error_e EEALoadImage(char *infile_path, bwimage_t *image); 

/* Dumps the image in the new tiff file */
error_e EEADumpImage(char *outfile_path, const bwimage_t *image); 

/* Deallocates the resources */
error_e EEAFreeImage(bwimage_t *image);

#ifdef __cplusplus
}
#endif

#endif//__EEA_TIFWRAP_H__
