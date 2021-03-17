#include <bi.h>

/**
 * Print a big integer object
 */
void bi_print(big_int* n) {
    if (n->sign == BIG_INT_NEGATIVE)
        printf("-");
    for (int32_t i = n->size - 1; i >= 0; i--)
        printf("%02x", n->buffer[i]);   
}
/**
 * Print a big integer object
 */
void bi_println(big_int* n) {
    bi_print(n);
    printf("\n");
}

