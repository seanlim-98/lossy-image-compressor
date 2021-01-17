/****************************************************************************
 *  COMP 40 HW4 Arith                                                       *
 *                                                                          *
 *  cvrgb.h                                                                 *
 *  CV-RGB Interface                                                        *
 *  This is the interface for the functions in the CV-RGB module. The       *
 *  functions in this module handle the RGB-CV transformation and the       *
 *  inverse transformations, and also the overall mapping across the pixmap *
 *  to apply the value conversions to each pixel of the image.              *
 *  Created By (UTLN): jlai03 & slim07                                      *
 *           On      : 10/25/2020                                           *
 *                                                                          *
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <a2methods.h>
#include <a2blocked.h>
#include <pnm.h>
#include <math.h>
#include <uarray.h>
#include "uarray2b.h"
#include "compress40.h"
#include "arith40.h"
#include <mem.h>
#include "dct.h" /* for Struct definition */

/* Function:    RGBtoCV
   Parameters:  Pnm_ppm image - Pnm_ppm containing image pixmap which will be
                                converted
                A2Methods_T - Methods suite used to access the pixmap
   Does:        Goes through the pixmap in image and converts each index's
                Pnm_rgb struct to a CV struct (red, green blue --> Y/Pb/Pr)
   Returns:     none - void
*/
extern void RGBtoCV(Pnm_ppm image, A2Methods_T methods);

/* Function:    convertRGBtoCV
   Parameters:  int col - integer indicating the column of the accessed pixel
                int row - integer indicating the row of the accessed pixel
                A2Methods_UArray2 uarray2 - the uarray2 of the pixmap being
                                            accessed
                void *elem - pointer to pixel being accessed (voided)
                void *cl - Pnm_ppm struct of image
   Does:        Mapping Helper Function which takes a given pixel in the image
                pixmap and converts the RGB struct to a CV struct. Converts the
                image format from RGB to CV format when ampped across the whole
                uarray2
   Returns:     none - void
*/
extern void convertRGBtoCV(int col, int row, A2Methods_UArray2 uarray2,
                                void *elem, void *cl);

/* Function:    CVtoRGB
   Parameters:  Pnm_ppm image - Pnm_ppm containing image pixmap which will be
                                converted
                A2Methods_T - Methods suite used to access the pixmap
   Does:        Goes through the pixmap in image and converts each index's
                CV struct to a Pnm_Rgb struct (Y/Pb/Pr --> red, green, blue)
   Returns:     none - void
*/
extern void CVtoRGB(Pnm_ppm image, A2Methods_T methods);

/* Function:    convertCVtoRGB
   Parameters:  int col - integer indicating the column of the accessed pixel
                int row - integer indicating the row of the accessed pixel
                A2Methods_UArray2 uarray2 - the uarray2 of the pixmap being
                                            accessed
                void *elem - pointer to pixel being accessed (voided)
                void *cl - Pnm_ppm struct of image
   Does:        Mapping Helper Function which takes a given pixel in the image
                pixmap and converts the CV struct to a RGB struct. Converts the
                image format from CV to RGB format when ampped across the whole
                uarray2
   Returns:     none - void
*/
extern void convertCVtoRGB(int col, int row, A2Methods_UArray2 uarray2,
                                void *elem, void *cl);
