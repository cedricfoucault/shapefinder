/* 
 * File:   centerfinder.h
 * Author: cedric
 *
 * Created on June 4, 2011, 12:04 PM
 */

#ifndef CENTERFINDER_H
#define	CENTERFINDER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tifwrap.h"
#include "freqprocess.h"
#include "fileread.h"
#include "cvwrap.h"

#ifndef MAX_PATH
#define MAX_PATH (256)
#endif

    int centerfinder();
#ifdef	__cplusplus
}
#endif

#endif	/* CENTERFINDER_H */

