#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define N 132907100998156430409098726803326611079501089072264941856793946139013747806628089359109985468368913942302076953803466423456091748297365270857097708168233

#define UINT_SZ	sizeof(uint8_t)

// Represents a big integer
struct big_int {
	bool sign; 			// 0 for positive integers, 1 for negative
	uint8_t* buffer;	// array of integers
	uint32_t size;		// size of the array
};
typedef struct big_int big_int;

// Represents the result of an euclidean division
struct big_int_eucl {
	big_int* q;	// Quotient
	big_int* r; // Remainder
};
typedef struct big_int_eucl big_int_eucl;

/* Memory management functions */
big_int* big_int_alloc();
big_int* big_int_create(int32_t value);
big_int* big_int_load(const char* s);
void big_int_destroy(big_int* a);
void big_int_cpy(big_int* dst, big_int* src);

/* Display functions */
void big_int_print(big_int* a);
void big_int_println(big_int* a);

/* Format function */
void big_int_reduce(big_int* a);
void big_int_shift(big_int* a, uint32_t shift);
void big_int_concat(big_int* a, big_int* b);
big_int* big_int_frame(big_int* a, uint32_t start, uint32_t end);

/* Math operators */
void big_int_abs(big_int* a);
void big_int_neg(big_int* a);
int32_t big_int_cmp(big_int* a, big_int* b);
void big_int_add(big_int* a, big_int* b);
void big_int_sub(big_int* a, big_int* b);
void big_int_mul(big_int* a, big_int* b);
void big_int_div(big_int* a, big_int* b);
void big_int_mod(big_int* a, big_int* b);
void big_int_pow(big_int* a, uint32_t e);
uint32_t big_int_len(big_int* a);
big_int* big_int_modexp(big_int* base, uint32_t e, big_int* mod);
big_int_eucl* big_int_eucl_div(big_int* a, big_int* b);