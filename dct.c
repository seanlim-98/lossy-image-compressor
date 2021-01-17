/****************************************************************************
 *  COMP 40 HW4 Arith                                                       *
 *                                                                          *
 *  dct.c                                                                   *
 *  Complete Discrete Cosine Transformation Implementation                  *
 *  Created By (UTLN): jlai03 & slim07                                      *
 *           On      : 10/25/2020                                           *
 *                                                                          *
 ****************************************************************************/
#include "dct.h"

/************************* COMPRESS FUNCTIONS ********************************/
extern void cosine_transform(int col, int row, Pnm_ppm image,
                             A2Methods_T methods, Bitword bitword)
{
        /* Asserts and get map of pixels from image */
        assert(methods != NULL);
        assert(image != NULL);
        A2Methods_UArray2 pixmap = image->pixels;
        assert(pixmap != NULL);

        /* Retrieve Y values at each pixel of 2x2 pixel block */
        float y1 = (*(CV *)(methods->at(pixmap, col, row)))->y;
        float y2 = (*(CV *)(methods->at(pixmap, col+1, row)))->y;
        float y3 = (*(CV *)(methods->at(pixmap, col, row+1)))->y;
        float y4 = (*(CV *)(methods->at(pixmap, col+1, row+1)))->y;

        /* Convert to abcd values and force within range */
        float b = (y4 + y3 - y2 - y1)/4.0;
        float c = (y4 - y3 + y2 - y1)/4.0;
        float d = (y4 - y3 - y2 + y1)/4.0;
        b = valid_range(b);
        c = valid_range(c);
        d = valid_range(d);

        /* Formulas for cosine transformation, and scaling within range */
        bitword->a = (unsigned)(roundf(((y4 + y3 + y2 + y1)/4.0) * 63)); // turn into 9-bit unsigned
        bitword->b = (signed)(roundf(b * 210));
        bitword->c = (signed)(roundf(c * 210));
        bitword->d = (signed)(roundf(d * 210));

        /* Free CV structs at each pixel of the pixel block */
        free(*(CV *)(methods->at(pixmap, col, row)));
        free(*(CV *)(methods->at(pixmap, col+1, row)));
        free(*(CV *)(methods->at(pixmap, col+1, row+1)));
        free(*(CV *)(methods->at(pixmap, col, row+1)));
}

extern float valid_range(float num)
{
        /* Force the float within the -0.3 to 0.3 range */
        if (num > 0.3) {
                return 0.3;
        }

        if (num < -0.3) {
                return -0.3;
        }
        return num;
}

/************************ DECOMPRESS FUNCTIONS *******************************/

extern void inverse_cosine(struct Pnm_ppm pixmap, UArray_T bitwords, A2Methods_T methods)
{
        assert(pixmap.pixels != NULL);
        for (int i = 0; i < UArray_length(bitwords); i++) {
                /* Iterate through bitwords, for each bitword store in 2x2 block
                in pixmap. Retrieve abcd values from the bitword. */
                Bitword curr = *(Bitword*)UArray_at(bitwords, i);
                float a = ((float)(curr->a) / 63);
                float b = ((float)(curr->b) / 210);
                float c = ((float)(curr->c) / 210);
                float d = ((float)(curr->d) / 210);

                /* Get chroma of PB and PR averages of Bitword, and force within
                range. */
                float pb = Arith40_chroma_of_index((unsigned) curr->pb_avg);
                float pr = Arith40_chroma_of_index((unsigned) curr->pr_avg);
                pb = valid_range(pb);
                pr = valid_range(pr);

                /* Allocate memory for CV Structs */
                CV cv_1 = malloc(sizeof(struct CV));
                CV cv_2 = malloc(sizeof(struct CV));
                CV cv_3 = malloc(sizeof(struct CV));
                CV cv_4 = malloc(sizeof(struct CV));
                assert(cv_1 != NULL);
                assert(cv_2 != NULL);
                assert(cv_3 != NULL);
                assert(cv_4 != NULL);

                /* Populate CV structs  after inverse cosine transformation */
                cv_1->pb = pb;
                cv_1->pr = pr;
                cv_1->y = a - b - c + d;

                cv_2->pb = pb;
                cv_2->pr = pr;
                cv_2->y = a - b + c - d;

                cv_3->pb = pb;
                cv_3->pr = pr;
                cv_3->y = a + b - c - d;

                cv_4->pb = pb;
                cv_4->pr = pr;
                cv_4->y = a + b + c + d;

                /* Find top-left pixel in pixmap to begin storing */
                int col = (i * 2) % pixmap.width;
                if (col == (int)pixmap.width) {
                        col = 0; /* goes to new row */
                }
                int row = ((i*2) / pixmap.width) * 2;

                /* Store CV Structs in the right pixels in block */
                *(CV *)(methods->at(pixmap.pixels, col, row)) = cv_1;
                *(CV *)(methods->at(pixmap.pixels, col + 1, row)) = cv_2;
                *(CV *)(methods->at(pixmap.pixels, col, row + 1)) = cv_3;
                *(CV *)(methods->at(pixmap.pixels, col + 1, row + 1)) = cv_4;
                free(curr);
        }
}
