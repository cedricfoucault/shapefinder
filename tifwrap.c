#include <stdio.h>
#include "stdlib.h"
#include "tifwrap.h"

bwimage_t *EEACreateImage()
{
  bwimage_t *retval=(bwimage_t*)malloc(sizeof(bwimage_t));
  retval->width=0;
  retval->height=0;
  retval->data=0;
  retval->rawdata=0;
  return retval;
}

error_e EEAFreeImage(bwimage_t *image)
{
  if(image->rawdata) free(image->rawdata);
  if(image->data) free(image->data);
  free(image);
  return EEA_OK;
}


error_e EEALoadImage(char *infile_path, bwimage_t *image){
  TIFF *infile;
  uint16 bps, photometric;
  uint16 *rcmap, *gcmap, *bcmap;
  uint8 *buf;
  unsigned int i, j;
  int ok;
  error_e retval=EEA_OK;
  
  infile=TIFFOpen(infile_path, "r");
  if(!infile){
    return EEA_ENOFILE;
  }
  /* phony do-while block used for error handling */
  do{
    TIFFGetField(infile, TIFFTAG_BITSPERSAMPLE, &bps);
    if(bps != 8) {
      retval=EEA_EBADBPS;
      break;
    }
    
    photometric=0;
    TIFFGetField(infile, TIFFTAG_PHOTOMETRIC, &photometric);
    //modif PW:
    if((photometric != PHOTOMETRIC_PALETTE)&&(photometric != PHOTOMETRIC_MINISWHITE)&&(photometric != PHOTOMETRIC_MINISBLACK)) 
    {
      retval=EEA_EBADPHOTOMETRIC;
      break;
    }
    
    
    if(photometric == PHOTOMETRIC_PALETTE)
    {
    /* This call redirects the pointers rcmap, gcmap, bcmap to the data structures created
       earlier by TIFFOpen */
             if(!TIFFGetField(infile, TIFFTAG_COLORMAP, &rcmap, &gcmap, &bcmap)){
              retval=EEA_ENOCOLORMAP;
              break;
        }
    ////(PW) we accept not-gray now
    ok=1;
    for(i=0; i<256; i++)
        {if((rcmap[i]!=gcmap[i]) || (rcmap[i]!=bcmap[i])) ok=0; }
    
    if(!ok)
             {  //retval=EEA_ENOTGRAY;break;
               //this line was added for console app, otherwise can be removed     
               printf("Warning: color image - will be converted to grayscale");    
             }    
    }
    TIFFGetField(infile, TIFFTAG_IMAGEWIDTH, &image->width);
    TIFFGetField(infile, TIFFTAG_IMAGELENGTH, &image->height);
    
    /* Start allocation */
    image->rawdata=(uint8*)malloc((image->width) * (image->height) * sizeof(uint8));
    image->data=(uint8**)malloc(image->height*sizeof(uint8*));
    buf=(uint8*)malloc(image->width * sizeof(uint8));
    if(!((image->rawdata)&&(image->data)&&(buf))){
      if(buf) free(buf);
      retval=EEA_ENOMEM;
      break;
    }
    /* Load the scanlines */
    for(i=0; i<image->height; i++){
      (image->data)[i]=image->rawdata + (i*image->width);
      TIFFReadScanline(infile, buf, i, 0);
      
      for(j=0; j<image->width; j++){
       
       ////(PW) try to make a 8-bit B&W image from whatever we got:
       switch(photometric)
         { 
                
        case PHOTOMETRIC_PALETTE:
            //average off all channels
          (image->data)[i][j]=((rcmap[buf[j]]>>8)+(gcmap[buf[j]]>>8)+(bcmap[buf[j]]>>8))/3; break;    
       
        case PHOTOMETRIC_MINISBLACK:
              (image->data)[i][j]=buf[j]; break;       
                    
        case PHOTOMETRIC_MINISWHITE:
              (image->data)[i][j]=255-buf[j]; break;                  
          
          }//switch
        
      }//for j
    }//for i
  } while(0);
  TIFFClose(infile);
  return retval;
}


error_e EEADumpImage(char *outfile_path, const bwimage_t *image){
  error_e retval=EEA_OK;
  TIFF *outfile;
  uint16 cmap[256];
  unsigned int i;

  outfile=TIFFOpen(outfile_path, "w");
  if(!outfile){
    return EEA_ENOFILE;
  }
  for(i=0; i<256; i++){
    cmap[i]=i<<8;
  }
  TIFFSetField(outfile, TIFFTAG_IMAGEWIDTH, image->width);
  TIFFSetField(outfile, TIFFTAG_IMAGELENGTH, image->height);
  TIFFSetField(outfile, TIFFTAG_BITSPERSAMPLE, 8);
  TIFFSetField(outfile, TIFFTAG_SAMPLESPERPIXEL, 1);
  TIFFSetField(outfile, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  TIFFSetField(outfile, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_PALETTE);
  TIFFSetField(outfile, TIFFTAG_COLORMAP, cmap, cmap, cmap);
  for(i=0; i<image->height; i++){
    TIFFWriteScanline(outfile, image->data[i], i, 0);
  }
  TIFFClose(outfile);
  return retval;
}

