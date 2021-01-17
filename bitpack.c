/****************************************************************************
 *  COMP 40 HW4 Arith                                                   *
 *                                                                          *
 *  bitpack.c                                                               *
 *  Complete Bitpack Implementation                                         *
 *  This is the implementation of the functions in the bitpack module.      *
 *  Created By (UTLN): jlai03 & slim07                                      *
 *           On      : 10/25/2020                                           *
 *                                                                          *
 ****************************************************************************/
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <bitpack.h>
#include "except.h"
#include "assert.h"

Except_T Bitpack_Overflow = { "Overflow" };

/******************* PRIVATE FUNCTIONS **************************************/
static uint64_t ushift_left(uint64_t word, unsigned offset);
static uint64_t ushift_right(uint64_t word, unsigned offset);
static int64_t sshift_left(int64_t word, unsigned offset);
static int64_t sshift_right(int64_t word, unsigned offset);

/******************* FIT FUNCTIONS *******************************************/
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        assert(width <= 64);
        /* Create mask, turn width number of bits on rightmost of sequence to
        1s and check that unsigned integer can 'fit*.*/
        uint64_t mask = ~0;
        mask = ushift_left(mask, width);
        mask = ~mask;
        return (n <= mask);
}

bool Bitpack_fitss( int64_t n, unsigned width)
{
        assert(width <= 64);
        /* Create mask, turn width number of bits on rightmost of sequence to
        1s and check that signed integer can 'fit*.*/
        int64_t min_mask = ~0;
        min_mask = sshift_left(min_mask, width - 1);
        int64_t max_mask = ~min_mask;
        return ((n >= min_mask) && (n <= max_mask));
}

/******************* GET FUNCTIONS *******************************************/
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= 64);
        assert(lsb + width <= 64);

        /* Create mask of all 0s except for the location of the word we are
        extracting. */
        uint64_t mask = ~0;
        mask = ushift_left(mask, 64 - width);
        mask = ushift_right(mask, 64 - width - lsb);

        /* Use AND operator on mask and the unsigned word to extract word */
        word = word & mask;
        word = ushift_right(word, lsb);
        return word;
}

int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= 64);
        assert(lsb + width <= 64);

        /* Extract word from the given unsigned word */
        uint64_t extract = Bitpack_getu(word, width, lsb);
        extract = ushift_left(extract, 64 - width);
        int64_t final = (int64_t)extract;
        final = sshift_right(final, 64 - width);

        return final;
}

/******************* REPLACE FUNCTIONS ***************************************/
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb,
                        uint64_t value)
{
        assert(width <= 64);
        assert(width + lsb <= 64);
        if (!Bitpack_fitsu(value, width)) {
                RAISE(Bitpack_Overflow);
        }

        /* Create mask, find the location in the sequence to do replacement */
        uint64_t mask = ~0;
        mask = ushift_left(mask, 64 - width);
        mask = ushift_right(mask, 64 - width - lsb);

        /* Shift sequence to be inserted into the right spot, and add into the
        existing bit sequence. */
        value = ushift_left(value, lsb);
        value = (value & mask);
        mask = ~mask;

        word = (word & mask);
        word = (word | value);
        return word;
}

uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,
                        int64_t value)
{
        assert(width <= 64);
        assert(width + lsb <= 64);
        if (!Bitpack_fitss(value, width)) {
                RAISE(Bitpack_Overflow);
        }

        /* Create mask, find the location in the sequence to do replacement */
        uint64_t mask = ~0;
        mask = ushift_left(mask, 64 - width);
        mask = ushift_right(mask, 64 - width - lsb);

        /* Shift sequence to be inserted into the right spot, and add into the
        existing bit sequence. */
        value = ushift_left(value, lsb);
        value = (value & mask);
        mask = ~mask;
        word = (word & mask);

        word = (word | value);
        return word;
}

/******************* SHIFT FUNCTIONS *****************************************/
/* Function:    ushift_left
   Parameters:  uint64_t word - The unsigned integer 'word' that we are
                                shifting.
                unsigned offset - amount at which the uint64_t will be shifted
                                  to the left by
   Does:        Takes the unsigned uint64_t 'word' and "shifts" the bits
                to the left by the given offset value.
   Returns:     uint64_t - the final unsigned bit sequence after the left shift
*/
static uint64_t ushift_left(uint64_t word, unsigned offset)
{
        /* Check that shift is within 64 bits */
        assert(offset <= 64);
        if (offset <= 63) {
                word = word << offset;
        } else {
                /* 'Resets' word to 0 if shift of 64 */
                word = 0;
        }
        return word;
}

/* Function:    ushift_right
   Parameters:  uint64_t word - The unsigned integer 'word' that we are
                                shifting.
                unsigned offset - amount at which the uint64_t will be shifted
                                  to the right by
   Does:        Takes the unsigned uint64_t 'word' and "shifts" the bits
                to the right by the given offset value.
   Returns:     uint64_t - the final unsigned bit sequence after  right shift
*/
static uint64_t ushift_right(uint64_t word, unsigned offset)
{
        /* Check that shift is within 64 bits */
        assert(offset <= 64);
        if (offset == 64) {
                /* 'Resets' word to 0 if shift of 64 */
                word = 0;
        } else {
                word = word >> offset;
        }
        return word;
}

/* Function:    sshift_left
   Parameters:  int64_t word - The signed integer 'word' that we are
                               shifting.
                unsigned offset - amount at which the uint64_t will be shifted
                                  to the left by
   Does:        Takes the signed uint64_t 'word' and "shifts" the bits
                to the left by the given offset value.
   Returns:     uint64_t - the final signed bit sequence after the left shift
*/
static int64_t sshift_left(int64_t word, unsigned offset)
{
        /* Check that shift is within 64 bits */
        assert(offset <= 64);
        if (offset <= 63) {
                word = word << offset;
        } else {
                /* 'Resets' word to 0 if shift of 64 */
                word = 0;
        }
        return word;
}

/* Function:    sshift_right
   Parameters:  int64_t word - The signed integer 'word' that we are
                               shifting.
                unsigned offset - amount at which the uint64_t will be shifted
                                  to the right by
   Does:        Takes the signed uint64_t 'word' and "shifts" the bits
                to the right by the given offset value.
   Returns:     int64_t - the final signed bit sequence after the right shift
*/
static int64_t sshift_right(int64_t word, unsigned offset)
{
        /* Check that shift is within 64 bits */
        assert(offset <= 64);
        if (offset <= 63) {
                word = word >> offset;
        } else if (word == 64) {
                word = 0;
        } else {
                word = ~0;
        }
        return word;
}

extern Except_T Bitpack_Overflow;
