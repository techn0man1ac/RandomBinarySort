#ifndef RANDOM_BINARY_SORT_H
#define RANDOM_BINARY_SORT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* --- Fast PRNG (Xorshift32) --- */
static inline uint32_t xorshift32(uint32_t *state) {
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

/* --- Stage A: Low-level Array-based Sort (Binary Search + memmove) --- */
void random_binary_sort_array(const int *src, int *dest, size_t n, uint32_t seed);

/* --- Stage B: Cartesian Tree (Treap) Node Structure --- */
typedef struct TreapNode {
    int key;
    uint32_t priority;
    struct TreapNode *left;
    struct TreapNode *right;
} TreapNode;

/* --- Stage B: Mathematical Evolution (Treap Dynamic Sort) --- */
void random_binary_sort_treap(const int *src, int *dest, size_t n, uint32_t seed);

#ifdef __cplusplus
}
#endif

#endif /* RANDOM_BINARY_SORT_H */