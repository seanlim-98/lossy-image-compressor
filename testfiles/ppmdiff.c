/****************************************************************************
 *  COMP 40 HW4 Arith                                                       *
 *                                                                          *
 *  ppmdiff.c                                                               *
 *  PPMDiff Implementation                                                  *
 *  This is the implementation of the ppmdiff prograkm that we used for     *
 *  testing similarity between PPM images (i.e. compressed and decompressed)*
 *  images.                                                                 *
 *  Created By (UTLN): jlai03 & slim07                                      *
 *           On      : 10/25/2020                                           *
 *                                                                          *
 ****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pnm.h>
#include <assert.h>
#include <a2methods.h>
#include <a2blocked.h>
#include <math.h>
#include "uarray2b.h"

/*********************** FUNCTION DECLARATIONS *******************************/
static bool check_dimensions(Pnm_ppm img1, Pnm_ppm img2);
static void compare_images(A2Methods_UArray2 pixel1, A2Methods_UArray2 pixel2,
                           A2Methods_T methods, unsigned denom1,
                           unsigned denom2);

int main(int argc, char *argv[])
{
        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods != NULL);

        FILE *fp1, *fp2;
        assert(argc == 3);
        /* Open 2 PPM files supplied ihe command line */
        fp1 = fopen(argv[1], "r");
        fp2 = fopen(argv[2], "r");
        assert(fp1 != NULL && fp2 != NULL);

        Pnm_ppm image1 = Pnm_ppmread(fp1, methods);
        Pnm_ppm image2 = Pnm_ppmread(fp2, methods);
        assert(image1 != NULL && image2 != NULL);

        unsigned denom1 = image1->denominator;
        unsigned denom2 = image2->denominator;
        /* Check that images do not differ in dimensions by more than 1 */
        if (!check_dimensions(image1, image2)) {
                fprintf(stderr, "Dimensions invalid!\n");
                fprintf(stdout, "1.0\n");
        } else {
                A2Methods_UArray2 pixel1 = image1->pixels;
                A2Methods_UArray2 pixel2 = image2->pixels;
                compare_images(pixel1, pixel2, methods, denom1, denom2);
        }
        Pnm_ppmfree(&image1);
        Pnm_ppmfree(&image2);
        fclose(fp1);
        fclose(fp2);
        return 0;
}

/* Function checks that two PPM images do not differ in width or height by
more than 1. Throws error in the main function if this condition isn't met. */
static bool check_dimensions(Pnm_ppm img1, Pnm_ppm img2)
{
        int height_diff = img1->height - img2->height;
        int width_diff = img1->width - img2->width;
        if (height_diff > 1 || height_diff < -1 || width_diff > 1 ||
                width_diff < -1) {
                return false;
        }
        return true;
}

/* This function compares each pixel by pixel for each PPM image and returns
us a calculated value that represents the similarlity of two roughyl simiarly
sized PPM images */
static void compare_images(A2Methods_UArray2 pixel1, A2Methods_UArray2 pixel2,
                        A2Methods_T methods, unsigned denom1, unsigned denom2)
{
        /* Dimensions - find shortest height and width */
        int width1 = methods->width(pixel1);
        int width2 = methods->width(pixel2);
        int height1 = methods->height(pixel1);
        int height2 = methods->height(pixel2);
        int shortest_width = (width1 > width2) ? width2 : width1;
        int shortest_height = (height1 > height2) ? height2 : height1;

        /* Compute Square Difference by iterating through image pixels */
        float running_sum = 0;
        for (int i = 0; i < shortest_width; i++) {
                for (int j = 0; j < shortest_height; j++) {
                        Pnm_rgb rgb1 = methods->at(pixel1, i, j);
                        Pnm_rgb rgb2 = methods->at(pixel2, i, j);
                        float r_diff = ((float)(rgb1->red) -
                                       (float)(rgb2->red))/denom1;
                        float g_diff = ((float)(rgb1->green) -
                                       (float)(rgb2->green))/denom1;
                        float b_diff = ((float)(rgb1->blue) -
                                       (float)(rgb2->blue))/denom1;
                        r_diff *= r_diff;
                        g_diff *= g_diff;
                        b_diff *= b_diff;
                        running_sum += (r_diff + g_diff + b_diff);
                }
        }
        float sqrt_sum;
        running_sum /= (3 * (shortest_width * shortest_height));
        sqrt_sum = sqrt(running_sum);
        /* Print similarity value to stdout */
        fprintf(stdout, "%1.4f\n", sqrt_sum);
}
