#include "bigint.h"

/**
 * Allocate a memory space to store big integers structure,
 * initialize it to 0
 */
big_int* big_int_alloc() {
	big_int* a = malloc(sizeof(big_int));
	a->sign = 0;
	a->buffer = malloc(UINT_SZ);
	a->buffer[0] = 0;
	a->size = 1;
	return a;
}

/**
 * Allocate and store a new 32bit integer into a 
 * big_int structure
 */
big_int* big_int_create(int32_t value) {
	big_int* a = big_int_alloc();
	if (value == 0)
		return a;

	if (value < 0) 
		big_int_neg(a);
	value = abs(value);

	uint32_t n = 0;
	uint32_t i = 0;
	while (value != 0) {
		// Reallocate buffer
		a->buffer = realloc(
			a->buffer, (i + 1) * UINT_SZ);

		// Split value into list of integers from 0 to 9
		n = value % 10;
		value = value / 10;

		// Store last number
		a->buffer[i] = n;
		i += 1;
	}
	a->size = i;

	return a;
}

/**
 * Un-allocate memory from big_int
 */
void big_int_destroy(big_int* a) {
	free(a->buffer);
	free(a);
}

/**
 * Load a big_int struct from a string
 * ex: big_int* a = big_int_load("1090192019029");
 */
big_int* big_int_load(const char* s) {
	big_int* a = big_int_alloc();

	int32_t len = strlen(s);
	for (int32_t i = len - 1; i >= 0; i--) {
		uint32_t n = (uint32_t) (s[i] ^ 0x30);

		big_int* tmp = big_int_create(n);
		big_int_shift(tmp, len - 1 - i);

		big_int_add(a, tmp);
		big_int_destroy(tmp);
	}

	return a;
}

/**
 * Copy the properties of the big_int src into
 * the big_int dst
 */
void big_int_cpy(big_int* dst, big_int* src) {
	dst->sign = src->sign;

	free(dst->buffer);
	dst->buffer = malloc(src->size);
	
	for (uint32_t i = 0; i < src->size; i++) {
		dst->buffer[i] = src->buffer[i];
	}
	dst->size = src->size;
}

/**
 * Print a big_int
 */
void big_int_print(big_int* a) {
	if (a->size == 0)
		return;

	if (a->sign == 1)
		printf("-");

	// Use different format for leading term to avoid leading zeroes
	for (int32_t i = a->size - 1; i >= 0; i--)
		printf("%01d", a->buffer[i]);
}

/**
 * Print a big int and a newline
 */
void big_int_println(big_int* a) {
	big_int_print(a);
	printf("\n");
}

/**
 * Remove useless leading zeroes
 */
void big_int_reduce(big_int* a) {
	int32_t i = a->size - 1;
	while (i > 0 && a->buffer[i] == 0) {
		a->buffer = realloc(
			a->buffer, i * UINT_SZ);
		i -= 1;
	}
	a->size = i + 1;
}

/**
 * Shift the big_int number
 * equivalent of a = a * (100 ^ shift)
 */
void big_int_shift(big_int* a, uint32_t shift) {
	if (shift == 0)
		return;

	a->buffer = realloc(
		a->buffer, (a->size + shift) * UINT_SZ);
	memset(a->buffer + a->size, 0, shift);

	for (uint32_t i = a->size + shift - 1; i >= shift; i--) {
		a->buffer[i] = a->buffer[i - shift];
		a->buffer[i - shift] = 0;
	}
	a->size = a->size + shift;
}

/**
 * Concatenate two big integers
 * a = a | b
 * ex: 18745 | 14 = 1874514
 */
void big_int_concat(big_int* a, big_int* b) {
	// Shift a to make place for b's digits
	big_int_shift(a, b->size);

	// Copy b's digits into a free place
	for (int32_t i = b->size - 1; i >= 0; i--)
		a->buffer[i] = b->buffer[i];

	big_int_reduce(a);
}

/**
 * Return a selected piece "frame" of a big int
 * ex: frame(18745, 0, 2) = 18
 */
big_int* big_int_frame(big_int* a, uint32_t start, uint32_t end) {
	big_int* result = malloc(sizeof(struct big_int));
	result->size = end - start;
	result->sign = 0;

	result->buffer = malloc(result->size * UINT_SZ);
	for (uint32_t i = 0; i < result->size; i++)
		result->buffer[i] = a->buffer[a->size - end + i];

	return result;
}

/**
 * Take the absolute value of a big_int
 */
void big_int_abs(big_int* a) {
	a->sign = 0;
}

/**
 * Return the negative value of a big_int
 * a = -a
 */
void big_int_neg(big_int* a) {
	a->sign = !a->sign;
}

/**
 * Compare two big ints
 * Return: -1 if a < b
 * 			1 if a > b
 * 			0 if a = b
 */ 
int32_t big_int_cmp(big_int* a, big_int* b) {
	// Compare signs
	if (a->sign == 1 && b->sign == 0) {
		return -1;
	} else if (a->sign == 0 && b->sign == 1) {
		return 1;
	}

	// Compare sizes
	if (a->size < b->size) {
		return -1;
	} else if (a->size > b->size) {
		return 1;
	}

	// Compare digit per digit
	for (int32_t i = a->size - 1; i >= 0; i--) {
		if (a->buffer[i] < b->buffer[i]) {
			return -1;
		} else if (a->buffer[i] > b->buffer[i]) {
			return 1;
		}
	}

	return 0;
}

/**
 * Add two big ints a & b and store the result in a
 * a = a + b
 */
void big_int_add(big_int* a, big_int* b) {
	uint8_t c = 0;
	bool carry = false;

	uint32_t i;
	for (i = 0; i < (uint32_t) fmax(a->size, b->size); i++) {
		uint32_t right = 0;
		if (i < b->size) {
			right = b->buffer[i];
		}

		uint32_t left = 0;
		if (i >= a->size) {
			// Extend a to store next digits
			a->buffer = realloc(
				a->buffer, (i + 1) * UINT_SZ);
			a->size += 1;
		} else {
			left = a->buffer[i];
		}

		c = left + right + carry;

		carry = false;
		if (c >= 10) {
			a->buffer[i] = c - 10;
			carry = true;
		} else {
			a->buffer[i] = c;
		}
 	}

 	// If we still have a carry, allocate one more space
 	if (carry) {
 		a->buffer = realloc(
 			a->buffer, (a->size + 1) * UINT_SZ);
 		a->buffer[a->size] = 1;
 		a->size += 1;
 	}
}

void big_int_sub(big_int* a, big_int* b) {
	if (big_int_cmp(a, b) == -1) {
		// if b > a, then a - b = - (b - a)
		
		// Copy b into another buffer that will be modified
		// by big_int_sub

		big_int* tmp = big_int_alloc();
		big_int_cpy(tmp, b);
	
		// Apply the sub
		big_int_sub(tmp, a);
		big_int_neg(tmp);

		// Copy the result in a
		big_int_cpy(a, tmp);

		// Destroy the temp buffer
		big_int_destroy(tmp);

		return;
	}

	int8_t c = 0;
	bool carry = false;

	uint32_t i;
	for (i = 0; i < (uint32_t) fmax(a->size, b->size); i++) {
		uint32_t right = 0;
		if (i < b->size) {
			right = b->buffer[i];
		}

		uint32_t left = 0;
		if (i >= a->size) {
			// Extend a to store next digits
			a->buffer = realloc(
				a->buffer, (i + 1) * UINT_SZ);
			a->size += 1;
		} else {
			left = a->buffer[i];
		}

		c = left - right - carry;

		carry = false;
		if (c < 0) {
			a->buffer[i] = 10 + c;
			carry = true;
		} else {
			a->buffer[i] = c;
		}
 	}

 	// If we still have a carry, allocate one more space
 	if (carry) {
 		a->buffer = realloc(
 			a->buffer, (a->size + 1) * UINT_SZ);
 		a->buffer[a->size] = 1;
 		a->size += 1;
 	}

 	// Remove leading zeroes
 	big_int_reduce(a);
}

/**
 * Multiply to big_int numbers a and b
 * set the result into a
 */
void big_int_mul(big_int* a, big_int* b) {
	big_int* result = big_int_alloc();

	big_int* tmp = NULL;
	uint32_t c = 0;
	for (int32_t i = a->size - 1; i >= 0; i--) {
		for (int32_t j = b->size - 1; j >= 0; j--) {
			// Compute the product of the heading terms
			c = a->buffer[i] * b->buffer[j];

			// Save it and shift it to the appropriate power of 10
			tmp = big_int_create(c);
			big_int_shift(tmp, i + j);

			// Add the result
			big_int_add(result, tmp);

			// Free the tmp buffer
			big_int_destroy(tmp);
		}
	}

	// Assign the sign
	if (a->sign != b->sign)
		big_int_neg(result);

	// Set the result
	big_int_cpy(a, result);

	// Destroy the result buffer
	big_int_destroy(result);
}

void big_int_div(big_int* a, big_int* b) {
	if (big_int_cmp(a, b) == -1) {
		big_int* zero = big_int_alloc();
		big_int_cpy(a, zero);
		big_int_destroy(zero);
		return;
	}

	big_int* count = big_int_alloc();
	big_int* one = big_int_create(1);

	while (big_int_cmp(a, b) > 0) {
		big_int_sub(a, b);
		big_int_add(count, one);
	}

	big_int_cpy(a, count);

	big_int_destroy(count);
	big_int_destroy(one);
}

uint32_t big_int_len(big_int* a) {
	return a->size - 1;
}

big_int_eucl* big_int_eucl_div(big_int* a, big_int* b) {
	uint32_t n = big_int_len(a);
	uint32_t m = big_int_len(b);

	big_int_eucl* result = malloc(sizeof(struct big_int_eucl));
	result->q = big_int_alloc();
	result->r = big_int_alloc();

	if (n < m) {
		// if n < m, then a < b, and a/b = 0, a%b = a
		big_int_cpy(result->r, a);
		return result;
	} else if (n == m) {
		// if n == m, no optimization needed
		big_int* tmp = big_int_alloc();
		big_int* one = big_int_create(1);

		while (true) {	// While tmp <= a
			big_int_add(result->q, one);	// 		q += 1
			big_int_add(tmp, b);			//		tmp += b

			if (big_int_cmp(tmp, a) <= 0)
				break;
		}

		big_int_cpy(result->r, a);
		big_int_sub(result->r, tmp);

		big_int_destroy(one);
		big_int_destroy(tmp);

		return result;
	} else {
		// Create a big int to represent 0 & 1
		big_int* zero = big_int_alloc();
		big_int* one = big_int_create(1);

		// Allocate a new a
		// he is dynamic and change with
		// the following operations
		big_int* new_a = big_int_alloc();
		big_int_cpy(new_a, a);

		
		while (true) {
			// Search for a big enough number to substract b
			big_int* frame = big_int_frame(new_a, 0, 1);
			// seq will contain the remaining digits outside the frame
			big_int* seq = big_int_frame(new_a, 1, new_a->size);

			uint32_t i = 1;
			while (big_int_cmp(frame, b) <= 0) {	// While frame < b
				// Extend frame
				i += 1;
				
				// Update it
				big_int_destroy(frame);
				frame = big_int_frame(new_a, 0, i);
			}
			
			// Update seq
			big_int_destroy(seq);
			seq = big_int_frame(new_a, i, new_a->size);

			// One we found the frame
			// find the quotient and the remainder
			// corresponding to frame / b

			// We need a temporary q for this division
			// we will be added to the overall q
			big_int* tmp_q = big_int_alloc();
			big_int* tmp = big_int_alloc();
			big_int* nxt = big_int_alloc();
			while (true) {
				big_int_add(tmp_q, one);		// 		q += 1
				big_int_add(tmp, b);			//		tmp += b

				// Compute next number to see if we
				// get higher than frame
				big_int_cpy(nxt, tmp);
				big_int_add(nxt, b);

				// if we are, quit
				if (big_int_cmp(nxt, frame) > 0)
					break;
			}

			// r = frame % b = frame - (q * b) = frame - tmp
			big_int_cpy(result->r, frame);
			big_int_sub(result->r, tmp);

			// If r = 0, add 0 to the quotient
			printf("q = ");
			big_int_print(tmp_q);
			printf("; r = ");
			big_int_print(result->r);
			printf("\n");

			if (big_int_cmp(result->r, zero) == 0)
				big_int_concat(result->q, zero);

			// Concat the new q with the overall quotient
			big_int_concat(result->q, tmp_q);

			// New a is the remainder + the seq
			big_int_cpy(new_a, result->r);
			big_int_concat(new_a, seq);

			// Free temporary variables			
			big_int_destroy(tmp_q);
			big_int_destroy(tmp);
			big_int_destroy(nxt);
			big_int_destroy(frame);
			big_int_destroy(seq);

			// If our current dividend is > to the divisor
			// we have nothing left to do
			if (big_int_cmp(new_a, b) <= 0 && seq->size == 0) {
				break;
			} else if (big_int_cmp(new_a, b) <= 0 && seq->size > 0) {
				for (uint32_t i = 0; i < seq->size; i++)
					big_int_concat(result->q, zero);
				big_int_cpy(result->r, new_a);
				break;
			}

		}

		// Destroy temp variables
		big_int_destroy(new_a);
		big_int_destroy(one);
		big_int_destroy(zero);

		// Reduce the result
		big_int_reduce(result->q);
		big_int_reduce(result->r);

		return result;
	}

	return NULL;
}

/**
 * Pow the big_int a to the exponent e (32bit uint)
 * a = a ^ e
 */
void big_int_pow(big_int* a, uint32_t e) {
	big_int* tmp = big_int_alloc();
	big_int_cpy(tmp, a);

	for (uint32_t i = 1; i < e; i++) {
		big_int_mul(a, tmp);
	}

	big_int_destroy(tmp);
}