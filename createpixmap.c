/****************************************************************************
 *  COMP 40 HW4 Arith                                                       *
 *                                                                          *
 *  createpixmap.c                                                          *
 *  Complete Createpixmap Implementation                                    *
 *  This is the implementation for the createpixmap implementation.         *
 *  Created By (UTLN): jlai03 & slim07                                      *
 *           On      : 10/25/2020                                           *
 *                                                                          *
 ****************************************************************************/
#include "createpixmap.h"

/* This function initializes and allocates memory to a 2D Array for our pixmap,
which will stored the decompressed pixels of the image. */
extern void allocate_mem(Pnm_ppm pixmap, A2Methods_T methods)
{
        assert(pixmap != NULL);
        assert(methods != NULL);
        /* Allocate memory for a new pixel map of a given blocksize and
        element size */
        pixmap->pixels = methods->new_with_blocksize(pixmap->width,
                                pixmap->height, sizeof(struct CV), 2);
}

extern struct Pnm_ppm create_image(FILE *input, A2Methods_T methods)
{
        unsigned height, width;
        /* Read the header of the output file from our compression function */
        int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u",
                                &width, &height);
        assert(read == 2);
        int c = getc(input);
        assert(c == '\n');

        /* Initialize a new Pnm_ppm struct for our decompression */
        struct Pnm_ppm pixmap = { .width = width, .height = height
                                , .denominator = 255, .pixels = NULL
                                , .methods = methods};

        return pixmap;
}
