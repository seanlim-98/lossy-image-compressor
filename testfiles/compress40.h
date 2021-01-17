/****************************************************************************
 *  COMP 40 HW4 Arith                                                       *
 *                                                                          *
 *  compress40.h                                                            *
 *  Compress40 Interface                                                    *
 *  This is the interface for the compress40 module. The functions in this  *
 *  module handle the overall compression and decompression of images       *
 *  passed into the command line. The functions here call helper functions  *
 *  from other modules each representing a step in the compression /        *
 *  decompression process.                                                  *
 *  Created By (UTLN): jlai03 & slim07                                      *
 *           On      : 10/25/2020                                           *
 *                                                                          *
 ****************************************************************************/
#include <stdio.h>

/*
 * The two functions below are functions you should implement.
 * They should take their input from the parameter and should
 *  write their output to stdout
 */

extern void compress40  (FILE *input);  /* reads PPM, writes compressed image */
extern void decompress40(FILE *input);  /* reads compressed image, writes PPM */
