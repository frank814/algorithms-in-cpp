# Algorithms in C++

Classic algorithms implemented from scratch in **C++17** — one folder per algorithm, every file tested, standard library only.

## Lessons

### [01 · Binary Search](01-binary-search/) — `binary_search.cpp`

Finds a target in a **sorted** list by cutting the search range in half each step (O(log n)).

- Check the middle — match → return the index; too small → keep the right half; too big → keep the left half.
- Empty range → not found (`-1`).
- Includes first / just-past-last position helpers for duplicated values.

### [02 · Selection Sort](02-selection-sort/) — `selection_sort.cpp`

Sorts in place by repeatedly picking the smallest remaining value (O(n²), no extra memory).

- A sorted prefix grows at the front while the rest stays unsorted.
- Each pass scans the unsorted part for its minimum and swaps it to the next open slot.
- After n passes the list is sorted.

### [03 · Recursion](03-recursion/) — `recursion.cpp`

A function solves smaller copies of the same problem until a base case answers directly.

- Each waiting call sits on the call stack until the deepest one returns, then frames unwind.
- Factorial guards against 64-bit overflow (`n > 20` → `0`).
- Fibonacci with a memo cache computes every value once — O(n).

### [04 · Quicksort](04-quicksort/) — `quicksort.cpp`

Partitions the list around a pivot and recurses on both sides (O(n log n) expected, in place).

- Pivot = median of first/middle/last elements, so splits stay balanced.
- Two indices walk inward, swapping values on the wrong side.
- Small ranges finish with insertion sort.

## Run a lesson

```bash
cd 01-binary-search
g++ -std=c++17 -O2 -Wall -Wextra -o binary_search binary_search.cpp
./binary_search
```

Each folder's `README.md` has its own command and expected output.

## Verify everything

```bash
for d in */; do
  src=$(ls "$d"*.cpp 2>/dev/null) || continue
  [ -z "$src" ] && continue
  g++ -std=c++17 -O2 -Wall -Wextra -o /tmp/alg "$src" \
    && /tmp/alg > /dev/null \
    && echo "PASS ${d%/}" || echo "FAIL ${d%/}"
done
```
