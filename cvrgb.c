/****************************************************************************
 *  COMP 40 HW4 Arith                                                   *
 *                                                                          *
 *  cvrgb.c                                                                 *
 *  CV-RGB Implementation                                                   *
 *  This is the implementation of the functions in the Cv-RGB module.       *
 *  Created By (UTLN): jlai03 & slim07                                      *
 *           On      : 10/25/2020                                           *
 *                                                                          *
 ****************************************************************************/
#include "cvrgb.h"

/********************** PRIVATE FUNCTIONS ***********************************/
static float scale_range(float num);

/********************** COMPRESSION FUNCTIONS *******************************/
extern void convertRGBtoCV(int col, int row, A2Methods_UArray2 uarray2,
                                void *elem, void *cl)
{
        (void) elem;
        assert(uarray2 != NULL);

        /* Get A2Methods struct and PPM image from closure */
        const struct A2Methods_T *methods = ((Pnm_ppm)cl)->methods;
        Pnm_ppm image = (Pnm_ppm) cl;
        Pnm_rgb rgb_data = methods->at(uarray2, col, row);

        /* Divide red, green, blue values by image denominator */
        float r = (float)(rgb_data->red) / (float) image->denominator;
        float g = (float)(rgb_data->green) / (float) image->denominator;
        float b = (float)(rgb_data->blue) / (float) image->denominator;

        /* Convert to Component Video values */
        CV processed_CV = malloc(sizeof(struct CV));
        assert(processed_CV != NULL);
        processed_CV->y = (0.299 * r) + (0.587 * g) + (0.114 * b);
        processed_CV->pb = (-0.168736 * r) - (0.331264 * g) + (0.5 * b);
        processed_CV->pr = (0.5 * r) - (0.418688 * g) - (0.081312 *b);

        /* Re-assign void pointer in uarray2 */
        *((CV *)methods->at(uarray2, col, row)) = processed_CV;
}

extern void RGBtoCV(Pnm_ppm image, A2Methods_T methods)
{
        assert(methods != NULL);
        assert(image != NULL);

        /* Get map of pixels from PPM Image */
        A2Methods_UArray2 pixmap = image->pixels;
        assert(pixmap != NULL);
        assert(methods->map_block_major != NULL);

        /* Iterate through PPM image and convert each pixel from an RGB
        struct to a CV struct */
        methods->map_block_major(pixmap, convertRGBtoCV, image);
}

/********************* DECOMPRESSION FUNCTIONS *******************************/
extern void convertCVtoRGB(int col, int row, A2Methods_UArray2 uarray2,
                                void *elem, void *cl)
{
        (void) elem;
        assert(uarray2 != NULL);

        /* Get A2Methods struct and PPM image from closure */
        const struct A2Methods_T *methods = ((Pnm_ppm)cl)->methods;
        Pnm_ppm image = (Pnm_ppm) cl;

        /* Retrieve CV struct and extract/convet values */
        CV cv_data = *((CV *)methods->at(uarray2, col, row));
        Pnm_rgb rgb_data = malloc(sizeof(struct Pnm_rgb));
        assert(rgb_data != NULL);

        /* Convert CV values to RGB format */
        float red = (1.0 * cv_data->y) + (0.0 * cv_data->pb) + (1.402 * cv_data->pr);
        float green = (1.0 * cv_data->y) - (0.344136 * cv_data->pb) - (0.7141136 * cv_data->pr);
        float blue = (1.0 * cv_data->y) + (1.772 * cv_data->pb) + (0.0 * cv_data->pr);

        /* Scale RGB values to within range */
        red = scale_range(red);
        green = scale_range(green);
        blue = scale_range(blue);

        rgb_data->red = roundf(red * image->denominator);
        rgb_data->green = roundf(green * image->denominator);
        rgb_data->blue = roundf(blue * image->denominator);

        /* Put RGB data into the pixmap */
        (*(Pnm_rgb)methods->at(uarray2, col, row)) = *rgb_data;
        free(cv_data);
        free(rgb_data);
}

extern void CVtoRGB(Pnm_ppm image, A2Methods_T methods)
{
        assert(methods != NULL);
        assert(image != NULL);

        /* Get map of pixels from PPM Image */
        A2Methods_UArray2 pixmap = image->pixels;
        assert(pixmap != NULL);
        assert(methods->map_block_major != NULL);

        /* Iterate through PPM image and convert each pixel from a CV
        struct to a RGB struct */
        methods->map_block_major(pixmap, convertCVtoRGB, image);
}

static float scale_range(float num)
{
        /* Force RGB values within range of 1.0 and 9.0 */
        if (num > 1.0) {
                return 1.0;
        }
        if (num < 0.0) {
                return 0.0;
        }
        return num;
}
