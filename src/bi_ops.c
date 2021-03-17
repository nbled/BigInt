/**
 * @file bi_ops.c
 * @brief Arithmetic operations on big_int
 * @author Nolan B
 * @version 1.1
 * @date 17 march 2021
 */

#include <bi.h>

/**
 * @brief Flip the big_int sign
 * 
 * n = -n
 *
 * @param big_int* n : target struct
 */
void bi_neg(big_int* n) {
    n->sign = !n->sign;
}

/**
 * Private function, adds only two POSITIVE
 * integers
 */
big_int* __bi_add(big_int* a, big_int* b) {
    big_int* result = bi_alloc();

    // Let's reallocate only once at the beginning
    uint32_t length = fmax(a->size, b->size);
    result->buffer = realloc(
        result->buffer, length * UINT_SZ);
    result->size = length;

    bool carry = false;
    uint32_t i;
    for (i = 0; i < length; i++) {
        uint8_t left = 0;
        if (i < a->size)
            left = a->buffer[i];

        uint8_t right = 0;
        if (i < b->size)
            right = b->buffer[i];

        // Sum byte block by byte block
        uint16_t word = left + right + carry;

        carry = false;
        if (word > 0xff) {
            result->buffer[i] = word & 0xff;
            carry = true;
        } else {
            result->buffer[i] = word;
        }
    }

    // If we still have a carry, allocate one more space
    if (carry) {
        result->buffer = realloc(
            result->buffer, (result->size + 1) * UINT_SZ);
        result->buffer[result->size] = 1;
        result->size += 1;
    }

    return result;
}

/**
 * Private function, substract only two POSITIVE
 * integers where a > b
 */
big_int* __bi_sub(big_int* a, big_int* b) {
    big_int* result = bi_alloc();

    uint32_t length = fmax(a->size, b->size);
    result->buffer = realloc(
        result->buffer, length * UINT_SZ);
    result->size = length;

    bool carry = false;

    uint32_t i;
    for (i = 0; i < length; i++) {      
        uint8_t left = 0;
        if (i < a->size) {
            left = a->buffer[i];
        }

        uint8_t right = 0;
        if (i < b->size) {
            right = b->buffer[i];
        }

        int16_t word = left - right - carry;

        carry = false;
        if (word < 0) {
            result->buffer[i] = 0x100 + word;
            carry = true;
        } else {
            result->buffer[i] = word;
        }
    }

    // If we still have a carry, allocate one more space
    if (carry) {
        result->buffer = realloc(
            result->buffer, (result->size + 1) * UINT_SZ);
        result->buffer[result->size] = 1;
        result->size += 1;
    }

    bi_reduce(result);
    return result;
}

/**
 * Private function, multiply two
 * positive integers < 256
 * (schoolbook mutliplication)
 */
big_int* __bi_mul_sb(big_int* a, big_int* b) {
    big_int* result = bi_alloc();

    uint16_t word = a->buffer[0] * b->buffer[0];
    if (word <= 0xff) {
        result->buffer[0] = word;
    } else {
        result->buffer = realloc(
            result->buffer, 2 * UINT_SZ);
        result->size = 2;
        
        result->buffer[0] = word & 0x00ff;
        result->buffer[1] = (word & 0xff00) >> 8;
    }

    return result;
}

/**
 * Private function, multiply two positive
 * integers, any size
 * (karatsuba algorithm)
 */
big_int* __bi_mul_karatsuba(big_int* a, big_int* b) {
    if (a->size == 1 && b->size == 1)
        return __bi_mul_sb(a, b);

    // Find the biggest common power of 2^8
    uint32_t m = fmax(a->size / 2, b->size / 2);

    // Split each number into x = x1 * 2^(8m) + x0
    big_int* x0;
    big_int* x1;

    if (m < a->size) {
        x1 = bi_frame(a, 0, a->size - m);
        x0 = bi_frame(a, a->size - m, a->size);
    } else {
        x1 = bi_alloc();
        x0 = bi_copy(a);
    }

    // y = y1 * 2 ^ (8m) + x0
    big_int* y0;
    big_int* y1;

    if (m < b->size) {
        y1 = bi_frame(b, 0, b->size - m);
        y0 = bi_frame(b, b->size - m, b->size);
    } else {
        y1 = bi_alloc();
        y0 = bi_copy(b);
    }

    // Compute z0, z1, z2
    big_int* z2 = __bi_mul_karatsuba(x1, y1);
    big_int* z0 = __bi_mul_karatsuba(x0, y0);
    
    big_int* sum1 = bi_add(x0, x1);
    big_int* sum2 = bi_add(y0, y1);

    big_int* z1 = __bi_mul_karatsuba(sum1, sum2);
    bi_move(z1, bi_sub(z1, z2));
    bi_move(z1, bi_sub(z1, z0));

    bi_lshift(z2, 2 * m);
    bi_lshift(z1, m);

    big_int* result = bi_add(z1, z2);
    bi_move(result, bi_add(result, z0));

    // Destroy all the variables
    bi_destroy(sum1); bi_destroy(sum2);
    bi_destroy(x1); bi_destroy(x0); 
    bi_destroy(y1); bi_destroy(y0);
    
    bi_destroy(z0);
    bi_destroy(z1);
    bi_destroy(z2);

    bi_reduce(result);
    return result;
}

/**
 * @brief Compare two big ints
 * 
 * Return: -1 if a < b (BIG_INT_LOWER)
 *          1 if a > b (BIG_INT_GREATER)
 *          0 if a = b (BIG_INT_EQUAL)
 * Complexity: Best case: O(1)
 *             Worst case: O(log a)
 *
 * @param big_int* a : first operand
 * @param big_int* b : second operand
 * @return comparaison flag
 */ 
int8_t bi_cmp(big_int* a, big_int* b) {
    // Compare signs
    if (a->sign == BIG_INT_NEGATIVE && b->sign == BIG_INT_POSITIVE) {
        return BIG_INT_SMALLER;
    } else if (a->sign == BIG_INT_POSITIVE && b->sign == BIG_INT_NEGATIVE) {
        return BIG_INT_GREATER;
    }

    // Compare sizes
    if (a->size < b->size) {
        return BIG_INT_SMALLER;
    } else if (a->size > b->size) {
        return BIG_INT_GREATER;
    }

    // Compare value per value
    for (int32_t i = a->size - 1; i >= 0; i--) {
        if (a->buffer[i] < b->buffer[i]) {
            return BIG_INT_SMALLER;
        } else if (a->buffer[i] > b->buffer[i]) {
            return BIG_INT_GREATER;
        }
    }

    return BIG_INT_EQUAL;
}

/**
 * @brief Add two big_int objects a and b
 *
 * Complexity: O(log max(a, b))
 *
 * @param big_int* a : first operand
 * @param big_int* b : second operand
 * @return pointer to the result a + b
 */
big_int* bi_add(big_int* a, big_int* b) {
    big_int* result = NULL;

    // If a > 0 and b > 0, a + b = a - |b|
    if (a->sign == BIG_INT_POSITIVE && b->sign == BIG_INT_NEGATIVE) {
        bi_neg(b);
        result = bi_sub(a, b);
        bi_neg(b);
    // If a < 0 and b > 0, a + b = b - |a|
    } else if (a->sign == BIG_INT_NEGATIVE && b->sign == BIG_INT_POSITIVE) {
        bi_neg(a);
        result = bi_sub(b, a);
        bi_neg(a);
    // If a < 0 and b < 0, a + b = -|a| + -|b| = -(|a| + |b|)
    } else if (a->sign == BIG_INT_NEGATIVE && b->sign == BIG_INT_NEGATIVE) {
        bi_neg(a); bi_neg(b);
        result = __bi_add(a, b);
        bi_neg(result);
        bi_neg(a); bi_neg(b);
    } else {
        result = __bi_add(a, b);
    }

    return result;
}

/**
 * @brief Substract two big_int objects a and b
 *
 * Complexity: O(log max(a, b))
 *
 * @param big_int* a : first operand
 * @param big_int* b : second operand
 * @return pointer to the result a - b
 */
big_int* bi_sub(big_int* a, big_int* b) {
    big_int* result = NULL;

    // If b < 0, then a - b = a - (-|b|) = a + |b|
    if (a->sign == BIG_INT_POSITIVE && b->sign == BIG_INT_NEGATIVE) {
        bi_neg(b);
        result = bi_add(a, b);
        bi_neg(b);
    // If a < 0, then a - b = -|a| - b = -(|a| + b)
    } else if (a->sign == BIG_INT_NEGATIVE && b->sign == BIG_INT_POSITIVE) {
        bi_neg(a);
        result = bi_add(a, b);
        bi_neg(result);
        bi_neg(a);
    // If a < 0 and b < 0, then a - b = -|a| - (-|b|) = -|a| + |b| = |b| - |a|
    } else if (a->sign == BIG_INT_NEGATIVE && b->sign == BIG_INT_NEGATIVE) {
        bi_neg(a); bi_neg(b);
        result = bi_sub(b, a);
        bi_neg(a); bi_neg(b);
    } else {
        // Here a > 0 and b > 0
        // If a < b, then a - b = -(b - a) (which is always true but useful in this case)
        if (bi_cmp(a, b) == BIG_INT_SMALLER) {
            result = __bi_sub(b, a);
            bi_neg(result);
        } else {
            result = __bi_sub(a, b);
        }
    }

    return result;
}

/**
 * @brief Multiply two integers a & b
 *
 * Complexity: O(n^log_3 (2))
 *
 * @param big_int* a : first operand
 * @param big_int* b : second operand
 * @return pointer to the result a * b
 */
big_int* bi_mul(big_int* a, big_int* b) {
    // Karatsuba function neglect sign
    // so we can give it directly a & b
    big_int* result = __bi_mul_karatsuba(a, b);

    // If a & b have different signs, then it's negative
    if (a->sign != b->sign)
        bi_neg(result);
    
    return result;
}

/**
 * @brief Compute euclidean division
 *
 * return a structure containing a pointer
 * to the quotient and the remainder
 * both are big int
 * the algorithm used is the one we can find
 * in schoolbooks
 * ex:      18495 | 43
 * -43*4 = -172|| |----
 *       =   129| | 430 <-quotient
 * -43*3 =  -129|
 *       =   0005
 * -43*0 =     -0
 *       =      5 <- remainder
 *
 * @param big_int* a : dividend
 * @param big_int* b : divisor
 * @return pointer to a big_int_eucl structure
 */
big_int_eucl* bi_eucl_div(big_int* a, big_int* b) {
    uint32_t n = a->size - 1;
    uint32_t m = b->size - 1;

    big_int_eucl* result = malloc(sizeof(struct big_int_eucl));

    if (n < m) {
        // if n < m, then a < b, and a/b = 0, a%b = a
        result->q = bi_alloc();
        result->r = bi_copy(a);
    } else if (n == m) {
        // if n == m, no optimization needed
        big_int* tmp = bi_alloc();
    
        uint8_t i = 0; // n and m are the same size so quotient can be up to 255
        while (true) {                          // While tmp <= a
            bi_move(tmp, bi_add(tmp, b));       //      tmp += b
            if (bi_cmp(tmp, a) == BIG_INT_GREATER) {
                bi_move(tmp, bi_sub(tmp, b));
                break;
            }
            i += 1;
        }

        result->q = bi_create(i);
        result->r = bi_sub(a, tmp);

        bi_destroy(tmp);
    } else {
        result->q = bi_alloc();

        // Allocate a new a
        // he is dynamic and change with
        // the following operations
        big_int* current = bi_alloc();

        int32_t index = a->size - 1;

        // We need a temporary q for this division
        // it will be added to the overall q
        big_int* next_number = bi_alloc();

        while (index >= 0) {
            // Add to our current dividend the next chunk of A
            big_int* next_chunk = bi_create(a->buffer[index]);
            bi_concat(current, next_chunk);
            bi_destroy(next_chunk);

            // Find the quotient by doing an "stupid" addition loop
            uint8_t tmp_q = 0; 
            while (true) {
                bi_move(next_number, bi_add(next_number, b));
                if (bi_cmp(next_number, current) == BIG_INT_GREATER) {
                    bi_move(next_number, bi_sub(next_number, b));
                    break;
                }
                tmp_q += 1;
            }

            // Concat the new q with the overall quotient
            big_int* tmp_q_bi = bi_create(tmp_q);
            bi_concat(result->q, tmp_q_bi);
            bi_destroy(tmp_q_bi);

            // r = current % b = current - (q * b) = current - next_number
            // Current is the new remainder
            bi_move(current, bi_sub(current, next_number));

            // Free temporary variables         
            bi_reset(next_number);

            index -= 1;
        }

        result->r = bi_sub(current, next_number);

        // Destroy temp variables
        bi_destroy(current);
        bi_destroy(next_number);

        // Reduce the result
        bi_reduce(result->q);
        bi_reduce(result->r);
    }

    return result;
}

/**
 * @brief Compute the quotient of the integer, division of a by b
 * @param big_int* a : dividend
 * @param big_int* b : divisor
 * @return pointer to the result a / b
 */
big_int* bi_div(big_int* a, big_int* b) {
    big_int_eucl* eucl = bi_eucl_div(a, b);
    big_int* result = bi_copy(eucl->q);
    bi_eucl_destroy(eucl);
    return result;
}

/**
 * @brief Compute the remainder of the integer, division of a by b
 * @param big_int* a : first operand
 * @param big_int* b : second operand
 * @return pointer to the result a % b
 */
big_int* bi_mod(big_int* a, big_int* b) {
    big_int_eucl* eucl = bi_eucl_div(a, b);
    big_int* result = bi_copy(eucl->r);
    bi_eucl_destroy(eucl);
    return result;
}

/**
 * @brief Compute b to the power of e using fast exponentation algorithm
 * @param big_int* b : basis
 * @param big_int* e : exponent
 * @return pointer to the result, b ^ e
 */
big_int* bi_exp(big_int* b, uint32_t e) {
    big_int* result; 
    if (e == 0)
        return bi_create(1);
    if (e == 1) {
        result = bi_copy(b);
        return result;
    }

    big_int* sq = bi_mul(b, b);
    if (e % 2 == 0) {
        result = bi_exp(sq, e / 2);
    } else {
        result = bi_exp(sq, (e - 1) / 2);
        bi_move(result, bi_mul(result, b));
    }
    bi_destroy(sq);

    return result;
}

/**
 * @brief Check if number is even
 * @param big_int* n : target struct
 * @return true if even, false if odd
 */
bool bi_is_even(big_int* n) {
    return !(n->buffer[0] & 1);
}

/**
 * @brief Fast modular exponentiation
 * @param big_int* b : basis
 * @param big_int* e : exponent
 * @param big_int* p : modulo
 * @return pointer to the result, b ^ e (mod p)
 */
big_int* bi_modexp(big_int* b, big_int* e, big_int* p) {
    big_int* zero = bi_alloc();
    big_int* one = bi_create(1);

    big_int* result = bi_create(1);

    big_int* e_cpy = bi_copy(e);
    big_int* b_cpy = bi_copy(b);

    while (bi_cmp(e_cpy, zero) != BIG_INT_EQUAL) {
        if (bi_is_even(e_cpy)) {
            // e = e / 2
            bi_rshift_bits(e_cpy, 1);
        } else {
            // e = (e - 1) / 2
            bi_move(e_cpy, bi_sub(e_cpy, one));
            bi_rshift_bits(e_cpy, 1);
        
            // result := z * b % p
            bi_move(result, bi_mul(result, b_cpy));
            bi_move(result, bi_mod(result, p));
        }

        // b = b^2 % p 
        bi_move(b_cpy, bi_mul(b_cpy, b_cpy));
        bi_move(b_cpy, bi_mod(b_cpy, p));
    }

    bi_destroy(e_cpy);
    bi_destroy(b_cpy);

    bi_destroy(zero);
    bi_destroy(one);

    return result;
}