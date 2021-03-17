#ifndef BIG_INT_HEADER
#define BIG_INT_HEADER

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define UINT_SZ sizeof(uint8_t)

#define _BIG_ENDIAN 1

#define BIG_INT_GREATER  1
#define BIG_INT_SMALLER -1
#define BIG_INT_EQUAL    0

#define BIG_INT_POSITIVE false
#define BIG_INT_NEGATIVE true

// Represents a variable size big integer
struct big_int {
	bool sign;			// number sign, 0 for positive, 1 for negative
	uint8_t* buffer;	// byte array
	uint32_t size;		// byte array size
};
typedef struct big_int big_int;

// Represents euclidean division result
struct big_int_eucl {
    big_int* q; // Quotient
    big_int* r; // Remainder
};
typedef struct big_int_eucl big_int_eucl;

// TODO:
//  UNITESTS
//  bi_from_i32
//  bi_from_i64
//  bi_cpy, bi_move
//  bi_add_mv, bi_%s_mv ....
/*
    bi_f_mv(big_int* f (f)(big_int* a, big_int* b), big_int* a, big_int* b) {
        big_int* c = f(a, b);
        bi_move(a, c);
    }
    + bitwise operations

    #define EQUAL(a, b) (bi_cmp(a, b) == BIG_INT_EQUAL)
*/

// Memory operations (bi_mem.c)
big_int* bi_alloc();
big_int* bi_create(int32_t value);
void bi_reset(big_int* n);
big_int* bi_from_buffer(const char* buff, int32_t size);
big_int* bi_copy(big_int* n);
void bi_move(big_int* dst, big_int* src);
void bi_reduce(big_int* n);
void bi_lshift(big_int* n, uint32_t shift);
void bi_rshift(big_int* n, uint32_t shift);
big_int* bi_frame(big_int* a, uint32_t start, uint32_t end);
void bi_concat(big_int* a, big_int* b);
void bi_destroy(big_int* n);
void bi_eucl_destroy(big_int_eucl* eucl);

// Display (bi_display.c)
void bi_print(big_int* n);
void bi_println(big_int* n);

// Math operations (bi_ops.c)
bool bi_is_even(big_int* n);
void bi_neg(big_int* n);
int8_t bi_cmp(big_int* a, big_int* b);
big_int* bi_add(big_int* a, big_int* b);
big_int* bi_sub(big_int* a, big_int* b);
big_int* bi_mul(big_int* a, big_int* b);
big_int_eucl* bi_eucl_div(big_int* a, big_int* b);
big_int* bi_div(big_int* a, big_int* b);
big_int* bi_mod(big_int* a, big_int* b);
big_int* bi_exp(big_int* b, uint32_t e);
big_int* bi_modexp(big_int* b, big_int* e, big_int* p);

// Binary operations (bi_bits.c)
void bi_set_bit(big_int* n, uint32_t pos, uint8_t bit);
bool bi_get_bit(big_int* n, uint32_t pos);
void bi_rshift_bits(big_int* n, uint32_t shift);

#endif