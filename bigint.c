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

		// Split value into list of integers from 0 to 99
		n = value % 100;
		value = value / 100;

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
		if ((i % 2 == 1 && len % 2 == 1) || (i % 2 == 0 && len % 2 == 0)) {
			n = n * 10;
		}

		big_int* tmp = big_int_create(n);
		big_int_shift(tmp, (len - 1 - i) / 2);

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

void big_int_concat(big_int* a, big_int* b) {
	uint32_t n = big_int_len(a);
	uint32_t m = big_int_len(b);

	// Compute the new buffer size
	uint32_t new_size = a->size + b->size;
	if (a->buffer[a->size - 1] < 10 && b->buffer[b->size - 1] < 10)
		new_size = new_size - 1;

	// Update buffer
	a->buffer = realloc(
		a->buffer, new_size);
	a->size = new_size;
	
	// Shift all the elements
	for (uint32_t i = a->size - 1; i >= 0; i--) {
		
	}
}

big_int* big_int_subb(big_int* a, uint32_t i, uint32_t j) {

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
	printf("%d", a->buffer[a->size - 1]);
	for (int32_t i = a->size - 2; i >= 0; i--)
		printf("%02d", a->buffer[i]);
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
	if (a->size < b->size) {
		return -1;
	} else if (a->size > b->size) {
		return 1;
	}

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
		if (c >= 100) {
			a->buffer[i] = c - 100;
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
			a->buffer[i] = 100 + c;
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

			// Save it and shift it to the appropriate power of 100
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
	return 2 * (a->size - 1) + (uint32_t) log10(a->buffer[a->size - 1]);
}

big_int_eucl* big_int_eucl_div(big_int* a, big_int* b) {
	uint32_t n = big_int_len(a);
	uint32_t m = big_int_len(b);

	big_int_eucl* result = malloc(sizeof(struct big_int_eucl));
	if (n < m) {
		// if n < m, then a < b, and a/b = 0, a%b = a
		result->q = big_int_create(0);
		result->r = big_int_alloc();
		big_int_cpy(result->r, a);
		return result;
	} else if (n == m) {
		// if n == m, no optimization needed
		big_int* tmp = big_int_alloc();
		big_int* one = big_int_create(1);

		result->q = big_int_alloc();
		while (true) {	// While tmp <= a
			big_int_add(result->q, one);	// 		q += 1
			big_int_add(tmp, b);			//		tmp += b

			if (big_int_cmp(tmp, a) <= 0)
				break;
		}

		result->r = big_int_alloc();
		big_int_cpy(result->r, a);
		big_int_sub(result->r, tmp);

		big_int_destroy(one);
		big_int_destroy(tmp);

		return result;
	} else {
		printf("not implemented\n");
		return result;
	}

	return NULL;
}

void big_int_mod(big_int* a, big_int* b) {
	big_int* tmp = big_int_alloc();
	big_int_cpy(tmp, a);
	
	big_int_div(tmp, b);
	big_int_mul(tmp, b);
	big_int_sub(a, tmp);

	big_int_destroy(tmp);
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

big_int* big_int_modexp(big_int* base, uint32_t e, big_int* mod) {
	big_int* c = big_int_create(1);
	if (big_int_cmp(mod, c) == 0)
		return big_int_alloc();

	for (uint32_t i = 0; i < e; i++) {
		big_int_mul(c, base);
		big_int_mod(c, mod);
	}

	return c;
}