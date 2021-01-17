/****************************************************************************
 *  COMP 40 HW4 Arith                                                        *
 *                                                                           *
 *  maptrans.h                                                               *
 *  Maptrans Interface                                                       *
 *  This is the interface for the functions in the maptrans module. The      *
 *  functions in this module handles the overall transformations and mapping *
 *  of the entire pixmap. transformation_map parses through the entire image *
 *  in 2x2 blocks and applies the linear transformations specified here and  *
 *  in dct to each block of pixels. block_chroma_map goes into each block    *
 *  and retrieves/converts the chroma values using data stored in each       *
 *  pixel.                                                                   *
 *  Created By (UTLN): jlai03 & slim07                                       *
 *           On      : 10/25/2020                                            *
 *                                                                           *
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
#include "dct.h"
#include "packwords.h"

/* Function:    block_chroma_map
   Parameters:  int col - column of top left pixel in block being accessed
                int row - row of top left pixel in block being accessed
                Pnm_ppm image - Pnm_ppm struct of pixmap being transformed
                A2Methods_T methods - Methods suite used to operate on pixmap
                Bitword word - Bitword struct representing pixmap block
   Does:        Takes a 2x2 pixmap block, takes the average of all the Pr and
                Pb pixel values in the given block. Uses
                Arith40_index_of_chroma to retrieve the indices for the Pr and
                Pb average, then stores the indices into Bitword word.
   Returns:     none - void
*/
extern void block_chroma_map(int col, int row, Pnm_ppm image,
                                A2Methods_T methods, Bitword word);

/* Function:    transformation_map
   Parameters:  Pnm_ppm image - Pnm_ppm struct of pixmap being transformed
                A2Methods_T methods - Method suite utilized to access and
                                      convert the given pixmap in Pnm_ppm image
   Does:        Takes a Pnm_ppm image, accesses its pixmap representation and
                compresses the data by mapping through each 2x2 block and
                converting it into Bitword representations. For each Bitword,
                we run it through a packing function that packs it into a 32
                bit codeword.
   Returns:     UArray_T of packed 32-bit code words after Bitwords have been
                created from each 2x2 pixel block and the Bitwords have been
                packed.
*/
extern UArray_T transformation_map(Pnm_ppm image, A2Methods_T methods);
