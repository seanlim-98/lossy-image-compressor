/****************************************************************************
 *  COMP 40 HW4 Arith                                                   *
 *                                                                          *
 *  bitpack.h                                                               *
 *  Complete Bitpack Interface                                              *
 *  This is the interface of the bitpack module. The functions in the       *
 *  module allow us to insert bits into, retrieve bits from a bit sequence, *
 *  and also check that a value can be represented by a specified number    *
 *  of bits. This was used to pack our bitwords during compression, and     *
 *  unpack our bitswords during decompression.                              *
 *  Created By (UTLN): jlai03 & slim07                                      *
 *           On      : 10/25/2020                                           *
 *                                                                          *
 ****************************************************************************/
#ifndef BITPACK_INCLUDED
#define BITPACK_INCLUDED
#include <stdbool.h>
#include <stdint.h>
#include "except.h"

/* Function:    Bitpack_fitsu
   Parameters:  uint64_t n - The unsigned integer that we are trying to test
                             for 'fit'.
                unsigned width - The width we are checking n fits within.
   Does:        Takes in an unsigned integer and checks if it can be fit within
                a given number of bits specified by width.
   Returns:     bool - indicates whether or not  an unsigned integer n can be
                       represented in width number of bits
*/
bool Bitpack_fitsu(uint64_t n, unsigned width);

/* Function:    Bitpack_fitss
   Parameters:  int64_t n - The signed integer we are trying to test for 'fit'.
                unsigned width - The width we are checking n fits within.
   Does:        Takes in a signed integer and checks if it can be fit within
                a given number of bits specified by width.
   Returns:     bool - indicates whether or not a signed integer can be
                       represented in width number of bits.
*/
bool Bitpack_fitss( int64_t n, unsigned width);

/* Function:    Bitpack_getu
   Parameters:  uint64_t word - The unsigned integer 'word' that we extract
                                the bits from.
                unsigned width - width of the bit sequence requested
                unsigned lsb - least significant bit (i.e. the right most "1"
                               of the requested bit sequence)
   Does:        Goes through the given unsigned uint64_t word and extracts the
                bitpack sub sequence indicated by the given width and lsb.
                Returns the requested sub sequence as an unsigned integer..
   Returns:     uint64_t - the requested bit sequence
*/
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb);

/* Function:    Bitpack_gets
   Parameters:  uint64_t word - The unsigned integer 'word' that we extract the
                                bits
                           from.
                unsigned width - width of the bit sequence requested
                unsigned lsb - least significant bit (i.e. the right most "1"
                               of the requested bit sequence)
   Does:        Goes through the given unsigned uint64_t word and extracts the
                bitpack sub-sequence indicated by the given width and lsb.
                Returns the requested sub sequence as a signed integer.
   Returns:     int64_t - the requested bit sequence as a signed integer.
*/
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb);

/* Function:    Bitpack_newu
   Parameters:  uint64_t word - The unsigned integer bit 'word' we extract bits
                                from.
                unsigned width - Width of the bit sequence requested
                unsigned lsb - least significant bit (i.e. the right most "1"
                               of the requested bit sequence)
                uint64_t value - The value of the unsigned bit sequence we are
                                trying to enplace within the word bit sequence.
   Does:        Goes through the given unsigned uint64_t word and finds the
                sequence of bits to replace with a specified value unsigned bit
                sequence indicated by value.
                Returns the updated bit sequence as an unsigned integer..
   Returns:     uint64_t - the updated bit sequence as an unsigned integer.
*/
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb,
                        uint64_t value);

/* Function:    Bitpack_news
Parameters:  uint64_t word - The unsigned integer bit 'word' we extract bits
                             from.
             unsigned width - width of the bit sequence requested
             unsigned lsb - least significant bit (i.e. the right most "1" of
                            the requested bit sequence)
             int64_t value - The value of the signed bit sequence we are
                             trying to enplace within the word bit sequence.
Does:        Goes through the given unsigned uint64_t word and finds the
             sequence of bits to replace with a specified signed bit sequence
             indicated by value. Returns the updated bit sequence as an
             unsigned integer..
Returns:     uint64_t - the updated bit sequence as an unsigned integer.
*/
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,
                        int64_t value);


extern Except_T Bitpack_Overflow;
#endif
