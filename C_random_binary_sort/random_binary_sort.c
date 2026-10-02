#include "random_binary_sort.h"
#include <stdlib.h>
#include <string.h>

/* =========================================================================
 * STAGE A: Array-based implementation with memmove()
 * ========================================================================= */

void random_binary_sort_array(const int *src, int *dest, size_t n, uint32_t seed) {
    if (n == 0 || !src || !dest) return;

    uint32_t rng = seed ? seed : 0x12345678U;

    // Working buffer for the unsorted pool
    int *pool = (int *)malloc(n * sizeof(int));
    if (!pool) return;
    memcpy(pool, src, n * sizeof(int));

    size_t remaining = n;

    for (size_t i = 0; i < n; i++) {
        // 1. Uniformly select an element from the pool in O(1)
        size_t r_idx = (size_t)(xorshift32(&rng) % remaining);
        int val = pool[r_idx];

        // O(1) extraction without shifting: swap with the last available element
        pool[r_idx] = pool[remaining - 1];
        remaining--;

        // 2. Binary search for insertion rank in dest[0 .. i-1]
        size_t low = 0, high = i;
        while (low < high) {
            size_t mid = low + (high - low) / 2;
            if (dest[mid] <= val) {
                low = mid + 1;
            } else {
                high = mid;
            }
        }
        size_t pos = low;

        // 3. Shift memory via hardware-level block copy
        if (pos < i) {
            memmove(&dest[pos + 1], &dest[pos], (i - pos) * sizeof(int));
        }
        dest[pos] = val;
    }

    free(pool);
}

/* =========================================================================
 * STAGE B: Mathematical Evolution — Treap (Cartesian Tree)
 * ========================================================================= */

static inline TreapNode *rotate_right(TreapNode *y) {
    TreapNode *x = y->left;
    y->left = x->right;
    x->right = y;
    return x;
}

static inline TreapNode *rotate_left(TreapNode *x) {
    TreapNode *y = x->right;
    x->right = y->left;
    y->left = x;
    return y;
}

static TreapNode *treap_insert(TreapNode *root, TreapNode *node) {
    if (!root) return node;

    if (node->key < root->key) {
        root->left = treap_insert(root->left, node);
        if (root->left->priority > root->priority) {
            root = rotate_right(root);
        }
    } else {
        root->right = treap_insert(root->right, node);
        if (root->right->priority > root->priority) {
            root = rotate_left(root);
        }
    }
    return root;
}

static void treap_inorder(const TreapNode *root, int *dest, size_t *idx) {
    if (!root) return;
    treap_inorder(root->left, dest, idx);
    dest[(*idx)++] = root->key;
    treap_inorder(root->right, dest, idx);
}

void random_binary_sort_treap(const int *src, int *dest, size_t n, uint32_t seed) {
    if (n == 0 || !src || !dest) return;

    uint32_t rng = seed ? seed : 0x87654321U;

    int *pool = (int *)malloc(n * sizeof(int));
    if (!pool) return;
    memcpy(pool, src, n * sizeof(int));

    /* Arena Allocation:
     * Allocate all nodes in a single contiguous block to eliminate
     * n individual malloc/free calls and maintain high CPU cache locality. */
    TreapNode *node_arena = (TreapNode *)malloc(n * sizeof(TreapNode));
    if (!node_arena) {
        free(pool);
        return;
    }

    TreapNode *root = NULL;
    size_t remaining = n;

    for (size_t i = 0; i < n; i++) {
        size_t r_idx = (size_t)(xorshift32(&rng) % remaining);
        int val = pool[r_idx];
        pool[r_idx] = pool[remaining - 1];
        remaining--;

        TreapNode *node = &node_arena[i];
        node->key = val;
        node->priority = xorshift32(&rng);
        node->left = NULL;
        node->right = NULL;

        root = treap_insert(root, node);
    }

    size_t out_idx = 0;
    treap_inorder(root, dest, &out_idx);

    free(node_arena);
    free(pool);
}