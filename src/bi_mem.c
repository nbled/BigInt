#include <bi.h>

/**
 * Allocate a memory space to store big integers structure,
 * initialize it to 0 (positive)
 */
big_int* bi_alloc() {
	big_int* n = malloc(sizeof(big_int));
	n->sign = BIG_INT_POSITIVE;
	n->size = 1;

	n->buffer = malloc(UINT_SZ);
	n->buffer[0] = 0;

	return n;
}

/**
 * Allocate and store a new 32bit integer into a 
 * big_int structure
 */
big_int* bi_create(int32_t value) {
	// Create a 0 big int
	big_int* n = bi_alloc();
	if (value == 0)
		return n;
	// Set sign bit
	if (value < 0) 
		bi_neg(n);
	value = abs(value);

	// Re-alloc and store
	n->buffer = realloc(n->buffer, 4 * UINT_SZ);
	n->buffer[0] = value & 0x000000ff;
	n->buffer[1] = (value & 0x0000ff00) >> 8;
	n->buffer[2] = (value & 0x00ff0000) >> 16;
	n->buffer[3] = (value & 0xff000000) >> 24;

	n->size = 4;
	bi_reduce(n);

	return n;
}

/**
 * Reset a big integer to 0
 */
void bi_reset(big_int* n) {
	n->buffer = realloc(
		n->buffer, 1 * UINT_SZ);
	n->buffer[0] = 0;
	n->size = 1;
	n->sign = BIG_INT_POSITIVE;
}

/**
 * Generate an integer from a byte-buffer
 * bi_from_buff("\x07\xde") = 2014
 */
big_int* bi_from_buffer(const char* buffer, int32_t size) {
	big_int* n = bi_alloc();
	n->buffer = realloc(n->buffer, size * UINT_SZ);
	n->size = size;

	for (int32_t i = 0; i < size; i++) {
#ifdef _BIG_ENDIAN
		n->buffer[i] = buffer[size - i - 1];
#else
		n->buffer[i] = buffer[i];
#endif	
	}

	bi_reduce(n);
	return n;
}

/**
 * Return a copy of a big_int object
 */
big_int* bi_copy(big_int* n) {
	big_int* result = bi_alloc();
	result->sign = n->sign;
	result->size = n->size;

	result->buffer = realloc(
		result->buffer, n->size * UINT_SZ);
	for (uint32_t i = 0; i < n->size; i++)
		result->buffer[i] = n->buffer[i];
	
	return result;
}

/**
 * Copy src in dst, and free src
 */
void bi_move(big_int* dst, big_int* src) {
	if (dst->buffer != NULL)
		free(dst->buffer);
	
	dst->buffer = malloc(src->size * UINT_SZ);
	dst->size = src->size;
	dst->sign = src->sign;

	memset(dst->buffer, 0, dst->size);
	for (uint32_t i = 0; i < src->size; i++)
		dst->buffer[i] = src->buffer[i];

	bi_destroy(src);
}

/**
 * Remove leading zeroes in a big_int
 */
void bi_reduce(big_int* n) {
	int32_t i = n->size - 1;
	while (i > 0 && n->buffer[i] == 0)
		i--;
	n->buffer = realloc(
		n->buffer, (i + 1) * UINT_SZ);
	n->size = i + 1;	
}

/**
 * Shift the digits to the left
 * equivalent to multiplying by 2**(8 * shift)
 */
void bi_lshift(big_int* n, uint32_t shift) {
	// Do not shift 0
	if (n->size == 1 && n->buffer[0] == 0)
		return;

	// No shift
	if (shift == 0)
		return;

	n->buffer = realloc(
		n->buffer, (n->size + shift) * UINT_SZ);
	memset(n->buffer + n->size, 0, shift);

	for (uint32_t i = n->size + shift - 1; i >= shift; i--) {
		n->buffer[i] = n->buffer[i - shift];
		n->buffer[i - shift] = 0;
	}
	n->size = n->size + shift;	
}

/**
 * Shift the digits to the right
 * equivalent to dividing by 2**(8 * shift)
 */
void bi_rshift(big_int* n, uint32_t shift) {
	// Do not shift 0
	if (n->size == 1 && n->buffer[0] == 0)
		return;

	// No shift
	if (shift == 0)
		return;

	for (uint32_t i = shift; i < n->size; i++) {
		n->buffer[i - shift] = n->buffer[i];
		n->buffer[i] = 0;
	}

	n->buffer = realloc(
		n->buffer, (n->size - shift) * UINT_SZ);
	n->size = n->size - shift;	
}

/**
 * Return a selected piece "frame" of a big int
 * ex: frame(18745, 0, 2) = 18
 */
big_int* bi_frame(big_int* n, uint32_t start, uint32_t end) {
	big_int* result = bi_alloc();

	result->size = end - start;
	result->buffer = realloc(
		result->buffer, result->size * UINT_SZ);
	result->sign = 0;

	for (uint32_t i = 0; i < result->size; i++)
		result->buffer[i] = n->buffer[n->size - end + i];

	return result;
}

/**
 * Concatenate two big integers
 * a = a | b
 * ex: 0xff | 0xed = 0xffed
 */
void bi_concat(big_int* a, big_int* b) {
	// Shift a to make place for b's digits
	bi_lshift(a, b->size);

	// Copy b's digits into a free place
	for (int32_t i = b->size - 1; i >= 0; i--)
		a->buffer[i] = b->buffer[i];
}

/**
 * Destroy a big_int object
 */
void bi_destroy(big_int* n) {
	free(n->buffer);
	free(n);
}

/**
 * Destroy a big_int_eucl object
 */
void bi_eucl_destroy(big_int_eucl* eucl) {
	bi_destroy(eucl->q);
	bi_destroy(eucl->r);
}