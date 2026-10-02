#!/usr/bin/env python3
"""
compare_and_demo.py

Visual trace and benchmark comparison suite.
Evaluates Randomized Binary Insertion Sort against:
  - Bubble Sort (standard O(n^2) baseline)
  - Insertion Sort (sequential O(n^2) baseline)
  - Merge Sort (pure-Python O(n log n))
  - Randomized QuickSort (pure-Python O(n log n))
"""

from __future__ import annotations

import bisect
import random
import time
from typing import Callable, Dict, List, Tuple

from random_binary_sort import random_binary_sort

# =====================================================================
# Benchmark Baselines
# =====================================================================


def bubble_sort(data: List[int]) -> List[int]:
    """Bubble Sort with early exit flag."""
    arr = data.copy()
    n = len(arr)
    for i in range(n - 1):
        swapped = False
        for j in range(n - i - 1):
            if arr[j] > arr[j + 1]:
                arr[j], arr[j + 1] = arr[j + 1], arr[j]
                swapped = True
        if not swapped:
            break
    return arr


def insertion_sort(data: List[int]) -> List[int]:
    """Sequential Insertion Sort."""
    arr = data.copy()
    for i in range(1, len(arr)):
        key = arr[i]
        j = i - 1
        while j >= 0 and arr[j] > key:
            arr[j + 1] = arr[j]
            j -= 1
        arr[j + 1] = key
    return arr


def merge_sort(data: List[int]) -> List[int]:
    """Recursive Merge Sort (pure Python)."""
    if len(data) <= 1:
        return data.copy()
    mid = len(data) // 2
    left = merge_sort(data[:mid])
    right = merge_sort(data[mid:])

    merged: List[int] = []
    i = j = 0
    while i < len(left) and j < len(right):
        if left[i] <= right[j]:
            merged.append(left[i])
            i += 1
        else:
            merged.append(right[j])
            j += 1
    merged.extend(left[i:])
    merged.extend(right[j:])
    return merged


def randomized_quicksort(data: List[int]) -> List[int]:
    """Randomized QuickSort (pure Python)."""
    if len(data) <= 1:
        return data.copy()
    pivot = random.choice(data)
    less = [x for x in data if x < pivot]
    equal = [x for x in data if x == pivot]
    greater = [x for x in data if x > pivot]
    return randomized_quicksort(less) + equal + randomized_quicksort(greater)


# =====================================================================
# Demonstration & Benchmarking
# =====================================================================


def run_visual_demo(sample: List[int]) -> None:
    """Prints a step-by-step trace of the algorithm in action."""
    print("=" * 80)
    print("1. STEP-BY-STEP VISUAL TRACE")
    print("=" * 80)
    print(f"Original Input : {sample}\n")
    print(f"{'Step':<6} | {'Picked':<8} | {'Remaining Pool':<30} | {'Sorted Accumulator'}")
    print("-" * 80)

    remaining = sample.copy()
    sorted_acc: List[int] = []
    step = 1

    while remaining:
        rand_idx = random.randrange(len(remaining))
        picked = remaining.pop(rand_idx)
        bisect.insort(sorted_acc, picked)

        pool_repr = str(remaining) if remaining else "[] (exhausted)"
        print(f"#{step:<5} | {picked:<8} | {pool_repr:<30} | {sorted_acc}")
        step += 1

    print("-" * 80)
    print(f"Final Result   : {sorted_acc}\n")


def measure(func: Callable[[List[int]], List[int]], dataset: List[int], runs: int = 3) -> float:
    """Measures average execution time in seconds."""
    total = 0.0
    for _ in range(runs):
        t0 = time.perf_counter()
        func(dataset)
        total += time.perf_counter() - t0
    return total / runs


def run_benchmarks() -> None:
    """Runs input size scaling and data distribution benchmarks."""
    algos: List[Tuple[str, Callable[[List[int]], List[int]]]] = [
        ("Bubble Sort", bubble_sort),
        ("Std Insertion", insertion_sort),
        ("Random Binary", random_binary_sort),
        ("Merge Sort", merge_sort),
        ("Rand QuickSort", randomized_quicksort),
    ]

    # Part A: Scaling benchmark
    sizes = [500, 1000, 5000, 10000, 50000, 100000]
    print("=" * 96)
    print("2. INPUT SIZE SCALING BENCHMARK (Uniform Random Integers)")
    print("=" * 96)
    header = f"{'Size (N)':<10} | " + " | ".join(f"{name:<14}" for name, _ in algos)
    print(header)
    print("-" * 96)

    for size in sizes:
        data = [random.randint(-100_000, 100_000) for _ in range(size)]
        row = [f"{size:<10}"]
        for name, fn in algos:
            runs = 1 if (size >= 2000 and "Bubble" in name) else 3
            avg_time = measure(fn, data, runs=runs)
            row.append(f"{avg_time:.5f} s     ")
        print(" | ".join(row))

    # Part B: Distribution benchmark
    n_fixed = 2000
    scenarios: Dict[str, List[int]] = {
        "Uniform Random": [random.randint(-10_000, 10_000) for _ in range(n_fixed)],
        "Already Sorted": list(range(n_fixed)),
        "Reverse Sorted": list(range(n_fixed, 0, -1)),
        "All Duplicates": [42] * n_fixed,
    }

    print("\n" + "=" * 96)
    print(f"3. DATA DISTRIBUTION BENCHMARK (Fixed Size N = {n_fixed:,})")
    print("=" * 96)
    dist_header = f"{'Distribution':<16} | " + " | ".join(f"{name:<14}" for name, _ in algos)
    print(dist_header)
    print("-" * 96)

    for dist_name, dataset in scenarios.items():
        row = [f"{dist_name:<16}"]
        for _, fn in algos:
            avg_time = measure(fn, dataset, runs=2)
            row.append(f"{avg_time:.5f} s     ")
        print(" | ".join(row))

    print("=" * 96)


if __name__ == "__main__":
    run_visual_demo([42, -5, 12, 0, 7, -5, 88, 3])
    run_benchmarks()
