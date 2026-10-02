"""
random_binary_sort.py

A lightweight implementation of Randomized Binary Insertion Sort.

The algorithm sorts an iterable by repeatedly drawing elements uniformly
at random without replacement and inserting them into an ordered accumulator
using binary search.
"""

from __future__ import annotations

import bisect
import random
from typing import Iterable, List, TypeVar

T = TypeVar("T")


def random_binary_sort(data: Iterable[T]) -> List[T]:
    """
    Sorts an iterable in ascending order via randomized binary insertion.

    Algorithmic Characteristics:
        - Comparisons: O(n log n) via binary search.
        - Memory Shifts: O(n^2) worst/average case due to list re-allocation.
        - Space Complexity: O(n) auxiliary space.
        - Order Sensitivity: Invariant to initial input ordering (eliminates
          adversarial sorted/reversed degradation).

    Args:
        data: Any iterable collection of comparable elements.

    Returns:
        A new list with elements sorted in ascending order.

    Example:
        >>> random_binary_sort([42, -5, 12, 0, 7, -5, 88, 3])
        [-5, -5, 0, 3, 7, 12, 42, 88]
    """
    remaining: List[T] = list(data)
    sorted_accumulator: List[T] = []

    # Repeatedly sample without replacement until the pool is empty
    while remaining:
        # Uniform random selection
        random_index = random.randrange(len(remaining))
        element = remaining.pop(random_index)

        # Binary search insertion into the sorted accumulator (C-accelerated)
        bisect.insort(sorted_accumulator, element)

    return sorted_accumulator


if __name__ == "__main__":
    import time

    sample_size = 50000
    dataset = [random.randint(-50_000, 50_000) for _ in range(sample_size)]

    start_time = time.perf_counter()
    sorted_data = random_binary_sort(dataset)
    elapsed_time = time.perf_counter() - start_time

    assert sorted_data == sorted(dataset), "Integrity check failed!"
    print(f"Successfully sorted {sample_size:,} elements in {elapsed_time:.4f} seconds.")
