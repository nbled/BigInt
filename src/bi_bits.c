/**
 * @file bi_bits.c
 * @brief Bitwise operations on big_int
 * @author Nolan B
 * @version 1.1
 * @date 17 march 2021
 */
#include <bi.h>

/**
 * @brief Return the number of bits taken by the big integer n
 * @param big_int* n : target struct
 * @return number of bits (uint32_t)
 */
uint32_t bi_bits(big_int* n) {
    return n->size * UINT_SZ * 8;
}

/**
 * @brief Set the bit at the position pos, pos 0 is the MSB
 * @param big_int* n : target struct
 * @param uint32_t pos : bit position
 * @param uint8_t bit : bit value (0 or 1)
 */
void bi_set_bit(big_int* n, uint32_t pos, uint8_t bit) {
    pos = bi_bits(n) - pos - 1;
    if (bit == 1)
      n->buffer[pos / 8] |= (1UL << (pos % 8));
    else
      n->buffer[pos / 8] &= ~(1UL << (pos % 8));
}

/**
 * @brief Get the bit at the position pos, pos 0 is the MSB
 * @param big_int* n : target struct
 * @param uint32_t pos : bit position
 * @return bit value (0 or 1)
 */
bool bi_get_bit(big_int* n, uint32_t pos) {
    pos = bi_bits(n) - pos - 1;
    return (n->buffer[pos / 8] >> (pos % 8)) & 1UL;
}

/**
 * @brief Shift all the bits to the right
 * @param big_int* n : target struct
 * @param uint32_t shift : right-shift
 */
void bi_rshift_bits(big_int* n, uint32_t shift) {
    bi_rshift(n, shift / 8);
    shift %= 8;

    uint8_t byte = 0;
    for (int32_t i = n->size - 1; i >= 0; i--) {
        uint8_t tmp = n->buffer[i] & ((uint8_t) pow(2, shift) - 1);
        n->buffer[i] = (n->buffer[i] >> shift) + (byte << (8 - shift));
        byte = tmp;
    }
}