/****************************************************************************
 *  COMP 40 HW4 Arith                                                       *
 *                                                                          *
 *  createpixmap.h                                                          *
 *  Createpixmap Interface                                                  *
 *  This the interface for the createpixmap module. The functions in this   *
 *  module handle the creation of a new Pnm_ppm struct and pixmap to store  *
 *  data at the start of the decompression process.                         *
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
#include "dct.h"

/* Function: allocate_mem
Parameters:  Pnm_ppm pixmap - The Pnm_ppm struct that we use to allocate memory
             for our pixmap (for decompression).
             A2Methods_T methods - The A2Methods_T methods suite with methods
             we use to allocate memory for our pixmap.
Does:        Use the dimension data in the pixmap struct, as well as the size
             data and the methods in the methods suite to allocate memory for
             the pixmap used to store our decompression data.
Returns:     none
*/
extern void allocate_mem(Pnm_ppm pixmap, A2Methods_T methods);

/* Function: create_image
Parameters:  FILE *input - pointer to input file
             A2Methods_T methods -The A2Methods_T methods suite with methods
             we allocate as the methods property of our initialized Pnm_ppm
             struct.
Does:        Takes in file pointers, retrieves data and populates a new Pnm_ppm
             struct.  Allocate methods as the methods property of
             initialized Pnm_ppm struct, and then returns the struct.
Returns:     A Pnm_ppm struct representing the decompressed image.
*/
extern struct Pnm_ppm create_image(FILE *input, A2Methods_T methods);
