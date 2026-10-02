#define _POSIX_C_SOURCE 199309L
#include "random_binary_sort.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int cmp_int(const void *a, const void *b) {
    int arg1 = *(const int *)a;
    int arg2 = *(const int *)b;
    return (arg1 > arg2) - (arg1 < arg2);
}

static void bubble_sort(int *arr, size_t n) {
    for (size_t i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (size_t j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
                swapped = 1;
            }
        }
        if (!swapped) break;
    }
}

static double get_time_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

static void run_distribution_test(size_t n) {
    printf("\n==================================================================================\n");
    printf(" IMMUNITY TEST: DATA DISTRIBUTION BENCHMARK (N = %zu)\n", n);
    printf("==================================================================================\n");

    const char *names[] = {"Uniform Random", "Pre-Sorted", "Reverse-Sorted", "All Duplicates"};
    int *src = (int *)malloc(n * sizeof(int));
    int *dest = (int *)malloc(n * sizeof(int));

    for (int t = 0; t < 4; t++) {
        if (t == 0) {
            uint32_t s = 1234;
            for (size_t i = 0; i < n; i++) src[i] = (int)(xorshift32(&s) % 100000);
        } else if (t == 1) {
            for (size_t i = 0; i < n; i++) src[i] = (int)i;
        } else if (t == 2) {
            for (size_t i = 0; i < n; i++) src[i] = (int)(n - i);
        } else {
            for (size_t i = 0; i < n; i++) src[i] = 42;
        }

        double t0 = get_time_sec();
        random_binary_sort_array(src, dest, n, 42);
        double t_arr = get_time_sec() - t0;

        t0 = get_time_sec();
        random_binary_sort_treap(src, dest, n, 42);
        double t_treap = get_time_sec() - t0;

        printf("  %-16s | Stage A (Array) : %8.5f s  | Stage B (Treap) : %8.5f s\n",
               names[t], t_arr, t_treap);
    }

    free(src);
    free(dest);
    printf("==================================================================================\n\n");
}

int main(void) {
    size_t sizes[] = {500, 2000, 10000, 30000, 50000, 100000, 500000, 1000000};
    size_t num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("==================================================================================\n");
    printf(" BENCHMARK: INPUT SIZE SCALING (C99, -O3, CLOCK_MONOTONIC)\n");
    printf("==================================================================================\n");
    printf("%-10s | %-12s | %-16s | %-16s | %-12s\n",
           "Size (N)", "Bubble Sort", "Stage A (Array)", "Stage B (Treap)", "libc qsort");
    printf("----------------------------------------------------------------------------------\n");

    for (size_t s = 0; s < num_sizes; s++) {
        size_t n = sizes[s];
        int *src = (int *)malloc(n * sizeof(int));
        int *dest = (int *)malloc(n * sizeof(int));
        int *check = (int *)malloc(n * sizeof(int));

        uint32_t seed = 42;
        for (size_t i = 0; i < n; i++) {
            src[i] = (int)(xorshift32(&seed) % 2000000) - 1000000;
        }

        double t_bubble = -1.0;
        if (n <= 10000) {
            memcpy(check, src, n * sizeof(int));
            double t0 = get_time_sec();
            bubble_sort(check, n);
            t_bubble = get_time_sec() - t0;
        }

        double t_stage_a = -1.0;
        if (n <= 100000) {
            double t0 = get_time_sec();
            random_binary_sort_array(src, dest, n, 1337);
            t_stage_a = get_time_sec() - t0;
        }

        double t0 = get_time_sec();
        random_binary_sort_treap(src, dest, n, 1337);
        double t_stage_b = get_time_sec() - t0;

        memcpy(check, src, n * sizeof(int));
        t0 = get_time_sec();
        qsort(check, n, sizeof(int), cmp_int);
        double t_qsort = get_time_sec() - t0;

        char b_str[32], a_str[32];
        if (t_bubble >= 0) sprintf(b_str, "%.5f s", t_bubble); else sprintf(b_str, "N/A (>10s)");
        if (t_stage_a >= 0) sprintf(a_str, "%.5f s", t_stage_a); else sprintf(a_str, "N/A (>5s)");

        printf("%-10zu | %-12s | %-16s | %-16.5f s | %-12.5f s\n",
               n, b_str, a_str, t_stage_b, t_qsort);

        free(src);
        free(dest);
        free(check);
    }

    run_distribution_test(50000);
    return 0;
}