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

/* Function Declarations */
static Pnm_ppm read_ppm(FILE *image, A2Methods_T methods);
static Pnm_ppm trim_image(Pnm_ppm image, A2Methods_T methods);
static void convertRGBtoCV(int col, int row, A2Methods_UArray2 uarray2, void *elem, void*cl);
static void RGBtoCV(Pnm_ppm image, A2Methods_T methods);
static void CVtoRGB(Pnm_ppm image, A2Methods_T methods);
static unsigned denominator;
static void valid_range(float *num);
static UArray_T read_packed_sequence(Pnm_ppm pixmap, FILE *input);
static UArray_T unpack_codewords(UArray_T codewords);
static void inverse_cosine(struct Pnm_ppm pixmap, UArray_T bitwords, A2Methods_T methods);
static struct Pnm_ppm make_image(FILE *input, A2Methods_T methods);
static float scale_range(float num);

int main(int argc, char* argv[])
{
        (void) argc;
        FILE *fp = fopen(argv[1], "r");
        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods != NULL);
        /* Get Pnm struct with read image and trim if necessary */
        Pnm_ppm image_ppm = read_ppm(fp, methods);
        assert(image_ppm != NULL);
        image_ppm = trim_image(image_ppm, methods);
        /* Map through PPM and convert from RGB to component-video and flip back */
        RGBtoCV(image_ppm, methods);
        CVtoRGB(image_ppm, methods);
        Pnm_ppmwrite(stdout, image_ppm);
        // UArray_T codewords = big_process_map(image_ppm, methods); /* Iterate through, process, return sequence of codewords */
        // assert(codewords != NULL);
        // print_compression(image_ppm, codewords); /* Print codewords to stdout */
        // UArray_free(&codewords);
        // Pnm_ppmfree(&image_ppm);
        // fclose(fp);

}

/********************** READ AND TRIM IMAGE **********************************/
static Pnm_ppm read_ppm(FILE *image, A2Methods_T methods)
{
        Pnm_ppm ppm_img = Pnm_ppmread(image, methods);
        assert(ppm_img != NULL);
        denominator = ppm_img->denominator;
        return ppm_img;
}

// static void populate_data(int col, int row, A2Methods_UArray2 uarray2,
//                                                         void *elem, void *cl)
// {
//         (void) elem;
//         Pnm_ppm image = (Pnm_ppm)cl;
//         const struct A2Methods_T *methods = ((Pnm_ppm)cl)->methods;
//
//         Pnm_rgb old_rgb = methods->at(image->pixels, col, row);
//         Pnm_rgb new_rgb = methods->at(uarray2, col, row);
//         new_rgb->red = old_rgb->red;
//         new_rgb->green = old_rgb->green;
//         new_rgb->blue = old_rgb->blue;
// }

/* Function to trim off last column or row of odd-dimensioned PPM */
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

        // methods->map_block_major(image->pixels, populate_data, image);
        methods->free(&(image->pixels));
        image->pixels = new_arr;
        return image;
}

/********************** RGB TO COMPONENT VIDEO *******************************/
typedef struct CV
{
    float y, pb, pr;
} *CV;

// /* Compression Step - convert from RGB to component-video */
static void convertRGBtoCV(int col, int row, A2Methods_UArray2 uarray2, void *elem, void *cl)
{
        // (void) uarray2;
        // (void) col;
        // (void) row;
        // assert(uarray2 != NULL);
        // // const struct A2Methods_T *methods = ((Pnm_ppm)cl)->methods;
        // Pnm_ppm image = (Pnm_ppm)cl;
        //
        // /* Pull RGB data */
        // Pnm_rgb rgb_data = (Pnm_rgb)elem;
        // float r = (float)(rgb_data->red) / (float)(image->denominator);
        // float g = (float)(rgb_data->green) / (float)(image->denominator);
        // float b = (float)(rgb_data->blue) / (float)(image->denominator);
        //
        // /* Convert to Component Video */
        // CV processed_CV = malloc(sizeof(struct CV));
        // assert(processed_CV != NULL);
        // processed_CV->y = (0.299 * r) + (0.587 * g) + (0.114 * b);
        // processed_CV->pb = (-0.168736 * r) - (0.331264 * g) + (0.5 * b);
        // processed_CV->pr = (0.5 * r) - (0.418688 * g) - (0.081312 * b);
        // /* Re-assign void pointer in uarray2 */
        // *(CV)elem = (*processed_CV);
        // free(processed_CV);

        (void) elem;
        assert(uarray2 != NULL);

        /* Get A2Methods struct and PPM image from closure */
        const struct A2Methods_T *methods = ((Pnm_ppm)cl)->methods;
        Pnm_ppm image = (Pnm_ppm)cl;
        Pnm_rgb rgb_data = elem;
        /* Divide red, green, blue values by image denominator */
        float r = (rgb_data->red) / (float)image->denominator;
        float g = (rgb_data->green) / (float)image->denominator;
        float b = (rgb_data->blue) / (float)image->denominator;

        /* Convert to Component Video values */
        CV processed_CV = malloc(sizeof(*processed_CV));
        assert(processed_CV != NULL);
        processed_CV->y = (0.299 * r) + (0.587 * g) + (0.114 * b);
        processed_CV->pb = (-0.168736 * r) - (0.331264 * g) + (0.5 * b);
        processed_CV->pr = (0.5 * r) - (0.418688 * g) - (0.081312 *b);
        /* Re-assign void pointer in uarray2 */
        *(CV*)methods->at(uarray2, col, row) = processed_CV;


}


static void RGBtoCV(Pnm_ppm image, A2Methods_T methods)
{
        assert(methods != NULL);
        assert(image != NULL);
        A2Methods_UArray2 pixmap = image->pixels;
        assert(pixmap != NULL);
        assert(methods->map_block_major != NULL);
        /* Iterate through PPM image and convert each RGB pixel */
        methods->map_block_major(pixmap, convertRGBtoCV, image);
}

// /* Decompression Step - convert from component-video to RGB */
static float scale_range(float num)
{
        if (num > 255.0) {
                return 255.0;
        }
        if (num < 0.0) {
                return 0.0;
        }
        return num;
}
//
static void convertCVtoRGB(int col, int row, A2Methods_UArray2 uarray2, void *elem, void *cl)
{
        // (void) uarray2;
        // (void) col;
        // (void) row;
        // // const struct A2Methods_T *methods = ((Pnm_ppm)cl)->methods;
        // Pnm_ppm image = (Pnm_ppm)cl;
        //
        // CV cv_data = (CV)elem;
        // Pnm_rgb rgb_data = malloc(sizeof(struct Pnm_rgb));
        // assert(rgb_data != NULL);
        //
        // float y = cv_data->y * (float)image->denominator;
        // float pb = cv_data->pb * (float)image->denominator;
        // float pr = cv_data->pr * (float)image->denominator;
        //
        // float red = (1.0 * y) + (0.0 * pb) + (1.402 * pr);
        // float green = (1.0 * y) - (0.344136 * pb) - (0.7141136 * pr);
        // float blue = (1.0 * y) + (1.772 * pb) + (0.0 * pr);
        //
        // scale_range(&red);
        // scale_range(&green);
        // scale_range(&blue);
        // // printf("%f, %f, %f\n", red, green, blue);
        //
        // rgb_data->red = (unsigned)(red);
        // rgb_data->green = (unsigned)(green);
        // rgb_data->blue = (unsigned)(blue);
        // // printf("%f, %u\n", red, rgb_data->red);
        // (*(Pnm_rgb)elem) = *rgb_data;
        // free(rgb_data);
        (void) elem;
        assert(uarray2 != NULL);

        /* Get A2Methods struct and PPM image from closure */
        const struct A2Methods_T *methods = ((Pnm_ppm)cl)->methods;
        Pnm_ppm image = (Pnm_ppm) cl;

        /* Retrieve CV struct and extract/convet values */
        CV cv_data = (*(CV *)methods->at(uarray2, col, row));
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

static void CVtoRGB(Pnm_ppm image, A2Methods_T methods)
{
        assert(methods != NULL);
        assert(image != NULL);
        A2Methods_UArray2 pixmap = image->pixels;
        assert(pixmap != NULL);
        assert(methods->map_block_major != NULL);
        /* Iterate through PPM image and convert each RGB pixel */
        methods->map_block_major(pixmap, convertCVtoRGB, image);
}


/******************* DISCRETE COSINE TRANSFORMATION **************************/
typedef struct Bitword {
        unsigned a;
        unsigned pb_avg, pr_avg;
        signed b, c, d;
} *Bitword;

static uint64_t pack_word(Bitword word)
{
        uint64_t result = 0;
        result = Bitpack_newu(result, 9, 23, word->a);
        result = Bitpack_news(result, 5, 18, word->b);
        result = Bitpack_news(result, 5, 13, word->c);
        result = Bitpack_news(result, 5, 8, word->d);
        result = Bitpack_newu(result, 4, 4, word->pb_avg);
        result = Bitpack_newu(result, 4, 0, word->pr_avg);
        return result;
}

static void cosine_transform(int col, int row, Pnm_ppm image,
                             A2Methods_T methods, Bitword bitword)
{
        /* Asserts and Set up */
        assert(methods != NULL);
        assert(image != NULL);
        A2Methods_UArray2 pixmap = image->pixels;
        assert(pixmap != NULL);

        /* Retrieve Y values */
        float y1 = (*(CV *)(methods->at(pixmap, col, row)))->y;
        float y2 = (*(CV *)(methods->at(pixmap, col+1, row)))->y;
        float y3 = (*(CV *)(methods->at(pixmap, col, row+1)))->y;
        float y4 = (*(CV *)(methods->at(pixmap, col+1, row+1)))->y;

        /* Convert and check range */
        float b = (y4 + y3 - y2 - y1)/4.0;
        float c = (y4 - y3 + y2 - y1)/4.0;
        float d = (y4 - y3 - y2 + y1)/4.0;

        valid_range(&b);
        valid_range(&c);
        valid_range(&d);

        /* Cosine Transformation */
        bitword->a = (unsigned)(roundf(((y4 + y3 + y2 + y1)/4.0) * 511)); // turn into 9-bit unsigned
        bitword->b = (signed)(roundf(b * 50));
        bitword->c = (signed)(roundf(c * 50));
        bitword->d = (signed)(roundf(d * 50));

        free((*(CV *)(methods->at(pixmap, col, row))));
        free(*(CV *)(methods->at(pixmap, col+1, row)));
        free(*(CV *)(methods->at(pixmap, col+1, row+1)));
        free(*(CV *)(methods->at(pixmap, col, row+1)));
}
//
static void valid_range(float* num)
{
        if (*num > 0.3) {
                *num = 0.3;
        }

        if (*num < -0.3) {
                *num = -0.3;
        }
}

/************************** MAPPING 2x2 BLOCK ********************************/
static void two_block_chroma_map(int col, int row, Pnm_ppm image, A2Methods_T methods, Bitword word)
{
        A2Methods_UArray2 pixmap = image->pixels;
        assert(pixmap != NULL);
        float avg_pb = 0;
        float avg_pr = 0;

        for (int i = row; i < row + 2; i++) {
                for (int j = col; j < col + 2; j++) {
                    CV temp = *(CV*) methods->at(pixmap, j, i);
                    avg_pr += temp->pr;
                    avg_pb += temp->pb;
                }
        }

        avg_pb /= 4.0;
        avg_pr /= 4.0;

        unsigned pb_bit_rep = Arith40_index_of_chroma(avg_pb);
        unsigned pr_bit_rep = Arith40_index_of_chroma(avg_pr);

        word->pb_avg = pb_bit_rep;
        word->pr_avg = pr_bit_rep;
}



static UArray_T big_process_map(Pnm_ppm image, A2Methods_T methods)
{
        int num_codewords = ((image->width) * (image->height)) / 4;
        UArray_T codewords = UArray_new(num_codewords, sizeof(uint64_t));
        int count = 0;
        for (unsigned int i = 0; i < image->height; i+= 2 ) {
                for (unsigned int j = 0; j < image->width; j+= 2 ) {
                        Bitword new_word = malloc(sizeof(*new_word));
                        assert(new_word != NULL);
                        two_block_chroma_map(j, i, image, methods, new_word); // Gives us the average chroma values
                        cosine_transform(j, i, image, methods, new_word);
                        uint64_t packed_word = pack_word(new_word); // Pack the word struct into code words
                        *((uint64_t *) UArray_at(codewords, count)) = packed_word;
                        free(new_word);
                        count++;
                }
        }

        return codewords;
}


/************************ COMPRESSION FUNCTION *******************************/

void print_compression(Pnm_ppm image, UArray_T sequence)
{
        printf("COMP40 Compressed image format 2\n%u %u", image->width, image->height);
        printf("\n");
        for (int i = 0; i < UArray_length(sequence); i++) {
                uint64_t codeword = *(uint64_t *)UArray_at(sequence, i);
                for (int i = 3; i >= 0; i--) {
                        char temp = Bitpack_getu(codeword, 8, i*8);
                        putchar(temp);
                }
        }
}

void compress40(FILE *input)
{
        assert(input != NULL);
        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods != NULL);
        /* Get Pnm struct with read image and trim if necessary */
        Pnm_ppm image_ppm = read_ppm(input, methods);
        assert(image_ppm != NULL);
        trim_image(image_ppm, methods);
        /* Map through PPM and convert from RGB to component-video */
        RGBtoCV(image_ppm, methods);
        UArray_T codewords = big_process_map(image_ppm, methods); /* Iterate through, process, return sequence of codewords */
        assert(codewords != NULL);
        print_compression(image_ppm, codewords); /* Print codewords to stdout */
        UArray_free(&codewords);
        Pnm_ppmfree(&image_ppm);
}


/*********************** DECOMPRESSION FUNCTION ******************************/

/* This function initializes and allocates memory to a 2D Array for our pixmap,
which will stored the decompressed pixels of the image. */
static void allocate_mem(Pnm_ppm pixmap, A2Methods_T methods)
{
        assert(pixmap != NULL);
        assert(methods != NULL);
        pixmap->pixels = methods->new_with_blocksize(pixmap->width,
                                pixmap->height, sizeof(struct CV), 2);
}

static struct Pnm_ppm make_image(FILE *input, A2Methods_T methods)
{
        unsigned height, width;
        int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u", &width, &height);
        assert(read == 2);
        int c = getc(input);
        assert(c == '\n');

        struct Pnm_ppm pixmap = { .width = width, .height = height
                                , .denominator = 255, .pixels = NULL
                                , .methods = methods};

        return pixmap;
}

void decompress40(FILE *input)
{
        assert(input != NULL);
        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods != NULL);
        struct Pnm_ppm pixmap = make_image(input, methods);
        allocate_mem(&pixmap, methods);

        /* Read the packed sequence of bits, and retrieve a sequence of codewords */
        UArray_T codewords = read_packed_sequence(&pixmap, input);
        UArray_T bitwords = unpack_codewords(codewords); /* Sequence of bitwords */
        inverse_cosine(pixmap, bitwords, methods); /* Iterate through bitwords, for each
                                         bitword store in 2x2 block in pixmap*/
        CVtoRGB(&pixmap, methods);
        Pnm_ppmwrite(stdout, &pixmap);

        methods->free(&(pixmap.pixels));
        UArray_free(&codewords);
        UArray_free(&bitwords);
}


static void inverse_cosine(struct Pnm_ppm pixmap, UArray_T bitwords, A2Methods_T methods)
{
        /* Iterate through bitwords, for each bitword store in 2x2 block
        in pixmap*/
        assert(pixmap.pixels != NULL);
        for (int i = 0; i < UArray_length(bitwords); i++) {
                /* Retrieve Bitword info  */
                Bitword curr = *(Bitword*)UArray_at(bitwords, i);
                // printf("%u\n", curr->a);
                float a = ((float)(curr->a) / 511);
                float b = ((float)(curr->b) / 50);
                float c = ((float)(curr->c) / 50);
                float d = ((float)(curr->d) / 50);

                float pb = Arith40_chroma_of_index((unsigned) curr->pb_avg);
                float pr = Arith40_chroma_of_index((unsigned) curr->pr_avg);

                valid_range(&pb);
                valid_range(&pr);

                /* Make CV Structs */
                CV cv_1 = malloc(sizeof(struct CV));
                CV cv_2 = malloc(sizeof(struct CV));
                CV cv_3 = malloc(sizeof(struct CV));
                CV cv_4 = malloc(sizeof(struct CV));
                assert(cv_1 != NULL);
                assert(cv_2 != NULL);
                assert(cv_3 != NULL);
                assert(cv_4 != NULL);

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
                *((CV *)methods->at(pixmap.pixels, col, row)) = cv_1;
                *(CV *)(methods->at(pixmap.pixels, col + 1, row)) = cv_2;
                *(CV *)(methods->at(pixmap.pixels, col, row + 1)) = cv_3;
                *(CV *)(methods->at(pixmap.pixels, col + 1, row + 1)) = cv_4;
                free(curr);
        }
}



/* Read the packed sequence of bits from the input stream, and return a
sequence of the codewords retrieved from them. */
static UArray_T read_packed_sequence(Pnm_ppm pixmap, FILE *input)
{
        int num_codewords = (pixmap->width * pixmap->height) / (2 * 2);
        UArray_T codewords = UArray_new(num_codewords, sizeof(uint64_t));
        int count = 0;
        for (int i = 0; i < num_codewords; i++) {
                uint64_t codeword = 0;
                /* For each codeword, we want to reverse the endian order and
                then put it into the seuqnece of codewords returned. */
                for (int j = 3; j >= 0; j--) {
                        uint64_t curr_byte = getc(input);
                        assert(sizeof(curr_byte) == 8); /* Check that codewords are complete */
                        codeword = Bitpack_newu(codeword, 8, j*8, curr_byte);
                }
                *(uint64_t*)UArray_at(codewords, count) = codeword;
                count++;
        }
        assert(UArray_length(codewords) == num_codewords); /* CRE if too few codewords for 2D array */
        return codewords;
}


/* Unpack codewords into a sequence of bitwords */
static UArray_T unpack_codewords(UArray_T codewords)
{
        int array_length = UArray_length(codewords);
        UArray_T bitwords = UArray_new(array_length, sizeof(Bitword));
        for (int i = 0; i < array_length; i++) {
                /* Initialize Bitword instance */
                Bitword curr_bitword = malloc(sizeof(*curr_bitword));
                assert(curr_bitword != NULL);
                uint64_t curr_codeword = *((uint64_t*) UArray_at(codewords, i));
                curr_bitword->pr_avg = Bitpack_getu(curr_codeword, 4, 0);
                curr_bitword->pb_avg = Bitpack_getu(curr_codeword, 4, 4);
                curr_bitword->d = Bitpack_gets(curr_codeword, 5, 8);
                curr_bitword->c = Bitpack_gets(curr_codeword, 5, 13);
                curr_bitword->b = Bitpack_gets(curr_codeword, 5, 18);
                curr_bitword->a = Bitpack_getu(curr_codeword, 9, 23);
                /* Store into new bitword sequence */
                *(Bitword *) UArray_at(bitwords, i) = curr_bitword;
        }
        return bitwords;
}
