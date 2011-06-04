/* 
 * File:   cvwrap.h
 * Author: cedric
 *
 * Created on June 4, 2011, 9:46 PM
 */

#ifndef CVWRAP_H
#define	CVWRAP_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdlib.h"
#include "tiffio.h"
#include "tifwrap.h"
#include <opencv/cv.h>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui_c.h>

    void iplImageToBwimage(bwimage_t **target, IplImage *src);


#ifdef	__cplusplus
}
#endif

#endif	/* CVWRAP_H */