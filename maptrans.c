/****************************************************************************
 *  COMP 40 HW4 Arith                                                       *
 *                                                                          *
 *  maptrans.c                                                              *
 *  Complete Maptrans Implementation                                        *
 *  This is the implementation of the functions in the maptrans module.     *
 *  Created By (UTLN): jlai03 & slim07                                      *
 *           On      : 10/25/2020                                           *
 *                                                                          *
 ****************************************************************************/
#include "maptrans.h"

extern void block_chroma_map(int col, int row, Pnm_ppm image,
                                A2Methods_T methods, Bitword word)
{
        A2Methods_UArray2 pixmap = image->pixels;
        assert(pixmap != NULL);
        /* Keep a running count for our PB and PR average values */
        float avg_pb = 0;
        float avg_pr = 0;

        /* Iterate through the 2x2 pixel block, retrieve the CV struct at each
        pixel and add the PB and PR values at each pixel to the running sums */
        for (int i = row; i < row + 2; i++) {
                for (int j = col; j < col + 2; j++) {
                    CV temp = *(CV*) methods->at(pixmap, j, i);
                    avg_pr += temp->pr;
                    avg_pb += temp->pb;
                }
        }
        /* Divide the summed PB and PR values by 4 to get average */
        avg_pb /= 4.0;
        avg_pr /= 4.0;

        /* Use Arith40_index_of_chroma to get an index from our chroma value */
        unsigned pb_bit_rep = Arith40_index_of_chroma(avg_pb);
        unsigned pr_bit_rep = Arith40_index_of_chroma(avg_pr);

        /* Add the chroma value representation to our Bitword */
        word->pb_avg = pb_bit_rep;
        word->pr_avg = pr_bit_rep;
}

extern UArray_T transformation_map(Pnm_ppm image, A2Methods_T methods)
{
        /* Get number of codewords to be create based on image dimensions, and
        initialize an array of codewords.*/
        int num_codewords = ((image->width) * (image->height)) / 4;
        UArray_T codewords = UArray_new(num_codewords, sizeof(uint64_t));
        int count = 0;

        /* Iterate through our image in 2x2 blocks of pixels */
        for (unsigned int i = 0; i < image->height; i+= 2 ) {
                for (unsigned int j = 0; j < image->width; j+= 2 ) {
                        /* Initialize a new Bitword (for each 2x2 block) */
                        Bitword new_word = malloc(sizeof(*new_word));
                        assert(new_word != NULL);
                        /* Extract chroma values from each block, store in
                        Bitword */
                        block_chroma_map(j, i, image, methods, new_word);
                        /* Run cosine transformation for each block, store
                        values post-transformation (a,b,c,d) in the Bitword */
                        cosine_transform(j, i, image, methods, new_word);
                        /* Pack the Bitword into a 32-bit code word */
                        uint64_t packed = pack_word(new_word);
                        /* Add the codeword to our array of codewords */
                        *((uint64_t *) UArray_at(codewords, count)) = packed;
                        free(new_word);
                        count++;
                }
        }
        /* Return array of packed 32-bit codewords */
        return codewords;
}
