/* 
 * File:   fileread.h
 * Author: cedric
 *
 * Created on June 4, 2011, 11:54 AM
 */

#ifndef FILEREAD_H
#define	FILEREAD_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>

    char *fgetstr(char *string, int n, FILE *stream);


#ifdef	__cplusplus
}
#endif

#endif	/* FILEREAD_H */

