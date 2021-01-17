/****************************************************************************
 *  COMP 40 HW4 Arith                                                       *
 *                                                                          *
 *  compress40.c                                                            *
 *  Complete Compress40 Implementation                                      *
 *  This is the implemmentation of the compress40 module.                   *
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
#include "cvrgb.h"
#include "dct.h"
#include "packwords.h"
#include "maptrans.h"
#include "createpixmap.h"

/********************* FUNCTION DECLARATIONS *********************************/
static Pnm_ppm read_ppm(FILE *image, A2Methods_T methods);
static Pnm_ppm trim_image(Pnm_ppm image, A2Methods_T methods);
static void print_compression(Pnm_ppm image, UArray_T sequence);

/********************** COMPRESSION FUNCTION *********************************/
void compress40(FILE *input)
{
        assert(input != NULL);
        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods != NULL);
        /* Get Pnm struct with read image and trim if necessary */
        Pnm_ppm image_ppm = read_ppm(input, methods);
        assert(image_ppm != NULL);
        image_ppm = trim_image(image_ppm, methods);
        /* Map through PPM and convert from RGB to component-video */
        RGBtoCV(image_ppm, methods);
        /* Iterate through, process, return sequence of codewords */
        UArray_T codewords = transformation_map(image_ppm, methods);
        assert(codewords != NULL);
        /* Print codewords to stdout */
        print_compression(image_ppm, codewords);
        UArray_free(&codewords);
        Pnm_ppmfree(&image_ppm);
}

/********************* DECOMPRESSION FUNCTION ********************************/
void decompress40(FILE *input)
{
        assert(input != NULL);
        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods != NULL);
        struct Pnm_ppm pixmap = create_image(input, methods);
        allocate_mem(&pixmap, methods);
        /* Read the packed sequence of bits, and retrieve a sequence of
        codewords. */
        UArray_T codewords = read_packed_sequence(&pixmap, input);
        /* Unpack codewords to get a sequence of Bitwords */
        UArray_T bitwords = unpack_codewords(codewords);
        /* Iterate through bitwords, for each Bitword, extract/convert data
        to CV data and store in 2x2 block in pixmap */
        inverse_cosine(pixmap, bitwords, methods);
        /* Convert CV struct of each pixel to RGB struct */
        CVtoRGB(&pixmap, methods);
        Pnm_ppmwrite(stdout, &pixmap);

        methods->free(&(pixmap.pixels));
        UArray_free(&codewords);
        UArray_free(&bitwords);
}

/********************** READ AND TRIM IMAGE **********************************/

/* This function reads a PPM image using Pnm_ppm read and returns a Pnm_ppm
struct representing the image. */
static Pnm_ppm read_ppm(FILE *image, A2Methods_T methods)
{
        Pnm_ppm ppm_img = Pnm_ppmread(image, methods);
        assert(ppm_img != NULL);
        return ppm_img;
}

/* Function to trim off last column or row of odd-dimensioned PPM. Turns
the widths and heights of all PPM images are even so that we can iterate
through them in 2x2 blocks. Allocate a new pixmap containing the new
dimensions to the original Pnm_ppm. */
static Pnm_ppm trim_image(Pnm_ppm image, A2Methods_T methods)
{
        assert(image != NULL);
        assert(image->width > 1 && image->height > 1);

        if (image->width % 2 != 0) {
                image->width = image->width - 1;
        }
        if (image->height % 2 != 0) {
                image->height = image->height - 1;
        }

        A2Methods_UArray2 new_arr = methods->new(image->width,
                                image->height, sizeof(struct Pnm_rgb));

        for (unsigned row = 0; row < image->height; row++) {
                for (unsigned col = 0; col < image->width; col++) {
                        Pnm_rgb old_rgb = methods->at(image->pixels, col, row);
                        Pnm_rgb new_rgb = methods->at(new_arr, col, row);
                        new_rgb->red = old_rgb->red;
                        new_rgb->green = old_rgb->green;
                        new_rgb->blue = old_rgb->blue;
                }
        }
        methods->free(&(image->pixels));
        image->pixels = new_arr;
        return image;
}


/* Print out a list of packed 32-bit codewords from a codeword sequence to
stdout. The codewords are each printed out in big-endian order. */
static void print_compression(Pnm_ppm image, UArray_T sequence)
{
        printf("COMP40 Compressed image format 2\n%u %u", image->width, image->height);
        printf("\n");
        /* Print in big-endian order */
        for (int i = 0; i < UArray_length(sequence); i++) {
                uint64_t codeword = *(uint64_t *)UArray_at(sequence, i);
                for (int i = 3; i >= 0; i--) {
                        char temp = Bitpack_getu(codeword, 8, i*8);
                        putchar(temp);
                }
        }
}
