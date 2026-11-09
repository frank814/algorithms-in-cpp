# Algorithms in C++

Algorithms in C++, built from scratch and tested — learning algorithms by building them.

## Files

### Binary Search — `binary_search.cpp`

Finds a target in a sorted list by cutting the search range in half on every step.

- Keeps two markers, `lo` and `hi`, around the part of the list still worth searching.
- Looks at the middle element: it is the target → done; the target is smaller → drop the whole right half; bigger → drop the whole left half.
- Repeats until the middle hits or the range becomes empty (not found → `-1`).
- Two extra helpers return the first or just-past-the-last position of a value, so duplicate-heavy lists behave predictably.

### Selection Sort — `selection_sort.cpp`

Sorts a list in place by repeatedly picking the smallest remaining value.

- The left part of the list is the *sorted prefix*; everything to its right is still unsorted.
- Each pass scans the unsorted part for its minimum value and swaps it into the first open slot.
- The prefix grows by one element per pass until the whole list is sorted.
- Needs no extra memory — the swaps happen inside the original list.

### Recursion — `recursion.cpp`

A function solves smaller copies of the same problem until a base case answers directly.

- Every call that cannot answer yet waits on the call stack; the deepest call (the base case) answers first.
- The waiting calls then unwind one frame at a time, each finishing its own piece of the answer.
- `factorial` guards against 64-bit overflow by returning `0` for `n > 20`.
- `fibonacci` keeps a small memo table, so each value is computed once instead of being recomputed down every branch.

### Quicksort — `quicksort.cpp`

Sorts in place by splitting around a pivot and sorting the two sides.

- The pivot is chosen as the median of the first, middle, and last elements — a balanced pick keeps the splits even.
- The partition step walks two pointers inward, swapping anything on the wrong side of the pivot, and finishes with the pivot in its final correct position.
- After one partition, everything left of the pivot is smaller and everything right is larger — so those two halves never interfere again.
- Recurses on both halves, stopping at empty or single-element ranges; a guard never lets it recurse over an unchanged whole array.

## Run

One example — the same two commands run any file in this repo:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -o run quicksort.cpp && ./run
```

Or run every file in one go:

```bash
for f in *.cpp; do
  g++ -std=c++17 -O2 -Wall -Wextra -o /tmp/alg "$f" \
    && /tmp/alg > /dev/null \
    && echo "PASS $f" || echo "FAIL $f"
done
```
