/****************************************************************************
 *  COMP 40 HW4 Arith                                                   *
 *                                                                          *
 *  packwords.c                                                             *
 *  Packwords Implementation                                                *
 *  This is the implementation of the packwords module.                     *
 *  Created By (UTLN): jlai03 & slim07                                      *
 *           On      : 10/25/2020                                           *
 *                                                                          *
 ****************************************************************************/
#include "packwords.h"

/*********************** COMPRESS FUNCTIONS **********************************/
extern uint64_t pack_word(Bitword word)
{
        /* Uses the bitpacking functions created in Bitpack.h/.c to pack the
        data from our Bitwords into a 32-bit code word */
        uint64_t result = 0;
        result = Bitpack_newu(result, 6, 26, word->a);
        result = Bitpack_news(result, 6, 20, word->b);
        result = Bitpack_news(result, 6, 14, word->c);
        result = Bitpack_news(result, 6, 8, word->d);
        result = Bitpack_newu(result, 4, 4, word->pb_avg);
        result = Bitpack_newu(result, 4, 0, word->pr_avg);
        return result;
}

/* Extracts all the codewords in reverse endian order from the packed bit
sequence we got after compression */
extern UArray_T read_packed_sequence(Pnm_ppm pixmap, FILE *input)
{
        /* Get the number of codewords expected to be read */
        int num_codewords = (pixmap->width * pixmap->height) / (2 * 2);
        UArray_T codewords = UArray_new(num_codewords, sizeof(uint64_t));
        int count = 0;
        for (int i = 0; i < num_codewords; i++) {
                uint64_t codeword = 0;
                /* For each codeword, we want to reverse the endian order and
                then put it into the sequence of codewords returned. */
                for (int j = 3; j >= 0; j--) {
                        /* Get each byte of the packed codewords in reverse
                        endian order */
                        uint64_t curr_byte = getc(input);
                        assert(sizeof(curr_byte) == 8);
                        /* Add each byte to the right place in the reversed
                        codeword */
                        codeword = Bitpack_newu(codeword, 8, j * 8, curr_byte);
                }
                /* Add reversed codeword to array */
                *(uint64_t*)UArray_at(codewords, count) = codeword;
                count++;
        }
        /* Raise CRE if too few codewords for 2D array */
        assert(UArray_length(codewords) == num_codewords);
        return codewords;
}

/******************** DECOMPRESS FUNCTIONS **********************************/

/* Unpack codewords into a sequence of Bitwords */
extern UArray_T unpack_codewords(UArray_T codewords)
{
        int array_length = UArray_length(codewords);
        UArray_T bitwords = UArray_new(array_length, sizeof(Bitword));
        for (int i = 0; i < array_length; i++) {
                /* Initialize Bitword instance */
                Bitword curr_bitword = malloc(sizeof(*curr_bitword));
                assert(curr_bitword != NULL);
                /* Extract each codeword from array */
                uint64_t curr_codeword = *((uint64_t*) UArray_at(codewords, i));
                /* use Bitpack functions to retrieve values from codeword, and
                store within a Bitword struct */
                curr_bitword->pr_avg = Bitpack_getu(curr_codeword, 4, 0);
                curr_bitword->pb_avg = Bitpack_getu(curr_codeword, 4, 4);
                curr_bitword->d = Bitpack_gets(curr_codeword, 6, 8);
                curr_bitword->c = Bitpack_gets(curr_codeword, 6, 14);
                curr_bitword->b = Bitpack_gets(curr_codeword, 6, 20);
                curr_bitword->a = Bitpack_getu(curr_codeword, 6, 26);
                /* Store Bitword into new bitword sequence */
                *(Bitword *) UArray_at(bitwords, i) = curr_bitword;
        }
        return bitwords;
}
