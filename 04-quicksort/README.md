# 04 · Quicksort

In-place sort that partitions the list around a pivot and recurses on both sides — O(n log n) expected.

## How it works

- Pick the pivot as the median of the first, middle, and last elements (keeps splits balanced).
- Partition: walk two indices inward from both ends, swapping values that sit on the wrong side.
- Recurse on both halves; small ranges finish with insertion sort.

## Run

```bash
g++ -std=c++17 -O2 -Wall -Wextra -o quicksort quicksort.cpp
./quicksort
```

Expected:

```
All quicksort tests passed.

before: 9 -3 4 4 0 7 -3 12 1 1 5 8 2 6 11 3 10 0 -7 13
after:  -7 -3 -3 0 0 1 1 2 3 4 4 5 6 7 8 9 10 11 12 13
```
