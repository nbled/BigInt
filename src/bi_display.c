/**
 * @file bi_display.c
 * @brief Display operations on big_int
 * @author Nolan B
 * @version 1.1
 * @date 17 march 2021
 */
#include <bi.h>

/**
 * @brief Print a big integer object
 * @param big_int* n : big_int to print
 */
void bi_print(big_int* n) {
    if (n->sign == BIG_INT_NEGATIVE)
        printf("-");
    for (int32_t i = n->size - 1; i >= 0; i--)
        printf("%02x", n->buffer[i]);   
}
/**
 * @brief Print a big integer object, and add a newline
 * @param big_int* n : big_int to print
 */
void bi_println(big_int* n) {
    bi_print(n);
    printf("\n");
}

