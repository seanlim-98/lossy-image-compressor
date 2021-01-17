/****************************************************************************
 *  COMP 40 HW4 Arith                                                   *
 *                                                                          *
 *  dct.h                                                                   *
 *  Discrete Cosine Transformation Interface                                *
 *  This is the Interface for the Discrete Cosine Transformation which is   *
 *  able to convert pixmap blocks between CVT structs and Bitword format.   *
 *  Cosine Transform utilizes other helper functions specified in the .c    *
 *  file to convert every 2x2 block in a pixmap to a new bitword map. The   *
 *  Inverse Cosine Function reverses this exact process.                    *
 *                                                                          *
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
#include "bitpack.h"
#include <mem.h>

#ifndef DCT
#define DCT

/********************** STRUCT DECLARATIONS **********************************/
/* Store the data that will eventually be packed into the codewords */
typedef struct Bitword {
        unsigned a;
        unsigned pb_avg, pr_avg;
        signed b, c, d;
} *Bitword;

/* Holds component-video data */
typedef struct CV
{
        float y, pb, pr;
} *CV;

/********************* COMPRESS FUNCTIONS ***********************************/

/* Function:    cosine_transform
   Parameters:  int col - column of top left pixel in block being accessed
                int row - row of top left pixel in block being accessed
                Pnm_ppm image - Pnm_ppm struct of pixmap being transformed
                A2Methods_T methods - Methods suite used to operate on pixmap
                Bitword bitword - Bitword struct representing pixmap block
   Does:        Accesses a given 2x2 pixmap block, uses method suite to access
                CV values from each pixel in the block, processes it using DCT
                transformation to get a, b, c, d, average Pb/Pr values that we
                then store in Bitwords (each representing a 2x2 block).
   Returns:    none - void
*/
extern void cosine_transform(int col, int row, Pnm_ppm image,
                             A2Methods_T methods, Bitword bitword);

/********************* DECOMPRESS FUNCTIONS *********************************/
/* Function:    inverse_cosine
   Parameters:  struct Pnm_ppm pximap - Pnm_ppm struct of pixmap being
                                        transformed
                UArray_T bitwords - Array of Bitwords that we iterate through,
                                    and for each, retrieve data that we pass
                                    into our inverse DCT transformation. Each
                                    Bitword is turned into a 2x2 block of
                                    pixels, each representing one CV struct.
                A2Methods_T methods - methods suite that we use to retrieve
                                      data from pixmap and apply inverse cosine
                                      transformations
   Does:        Accesses a given 2x2 pixmap block, uses method suite to access
                CV values from each pixel in the block, processes it using DCT
                transformation to get a, b, c, d, average Pb/Pr values that we
                then store in Bitwords (each representing a 2x2 block).
   Returns:     none - void
*/
extern void inverse_cosine(struct Pnm_ppm pixmap, UArray_T bitwords,
                           A2Methods_T methods);

/* Function:    valid_range
   Parameters:  float num - the float that we want to force within range
   Does:        Takes a float number and check that it is within -0.3 to 0.3.
                If it is out of bounds, we force the float within range and
                return it.
   Returns:     float - number forced into range [-0.3,0.3]
*/
extern float valid_range(float num);

#endif
