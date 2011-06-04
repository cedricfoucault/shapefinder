/* 
 * File:   shapefinder.h
 * Author: cedric
 *
 * Created on June 4, 2011, 1:30 PM
 */

#ifndef SHAPEFINDER_H
#define	SHAPEFINDER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tifwrap.h"
#include "freqprocess.h"

#ifndef MAX_PATH
#define MAX_PATH (256)
#endif

    int shapefinder();
    
#ifdef	__cplusplus
}
#endif

#endif	/* SHAPEFINDER_H */

