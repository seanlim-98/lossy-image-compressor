/****************************************************************************
 *  COMP 40 HW4 Arith                                                       *
 *                                                                          *
 *  packwords.h                                                             *
 *  Packwords Implementation                                                *
 *  The Interface for the packwords module. The functions in this module    *
 *  handle the packing and unpacking of the 32 bit codewords using the data *
 *  stored in our pixmap. The pack_word function is used in compress to     *
 *  pack bitword data into codeowrds, and the read_packed_sequence and      *
 *  unpack_codewords functions are used in decompression to unpack the      *
 *  codewords into bitword data.                                            *
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

/*********************** COMPRESS FUNCTIONS **********************************/

/* Function:    pack_word
   Parameters:  Bitword word - Bitword representation of a pixmap block
   Does:        Takes a Bitword representation and bitpacks it into an
                unsigned 32 bit code-word
   Returns:     uint64_t - unsigned 32 bit code-word
*/
extern uint64_t pack_word(Bitword word);

/* Function:    read_packed_sequence
   Parameters:  Pnm_ppm pixmap - Pnm_ppm struct representing the initial image
                                 we want to decompress to.
                FILE *input - pointer to file input (i.e. the sequence of bits
                              representing the compressed image)
   Does:        Reads the sequence of bits returned after compression and
                extracts an array of codewords from it using the bitpack
                functions.
   Returns:     UArray_T - an array of unpacked codewords
*/
extern UArray_T read_packed_sequence(Pnm_ppm pixmap, FILE *input);

/******************** DECOMPRESS FUNCTIONS **********************************/

/* Function:    unpack_codewords
   Parameters:  UArray_T codeword - UArray of bitpacked codewords
   Does:        Takes a UArray of bitpacked codewords and converts each
                codeword to a Bitword struct by retrieving data using the
                Bitpack functions.
   Returns:    UArray_T - an array of Bitwords (after unpacking codewords)
*/
extern UArray_T unpack_codewords(UArray_T codewords);
