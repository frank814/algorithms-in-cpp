# 04 · Quicksort

> Algorithm **4** of the *Algorithms in C++* series — an original,
> from-scratch implementation with a full test suite. C++17, zero dependencies.

Quicksort sorts by **partitioning**: pick a pivot, shuffle the array so every
value on the left side is `<= pivot` and every value on the right is
`>= pivot`, then recurse on each side. Done well, the splits stay balanced and
the work halves each level — O(n log n) expected. This implementation adds the
defences real code needs: a median-of-three pivot, Hoare's partition scheme,
an insertion-sort cutoff for small ranges, and tail-recursion elimination so
the stack stays shallow.

---

## The core idea

Maintain this invariant across every partition call:

> After `partition(v, lo, hi, pivot)` returns split point `j`,
> `v[lo..j] <= pivot <= v[j+1..hi]`, and both sides are strictly smaller than
> the original range.

1. If the range is at most `kInsertionCutoff` (16) elements, finish it with
   insertion sort — partitioning small ranges costs more than it saves.
2. Otherwise choose the pivot as the **median of three**: first, middle, and
   last elements. Sorted or reversed input then still splits near the middle
   instead of degenerating.
3. Hoare partition: two indices walk inward from both ends, swapping
   out-of-place values until they cross. Unlike Lomuto, it degrades gracefully
   when every element is equal.
4. Recurse into the **smaller** side, loop on the larger one. The loop keeps
   stack depth at O(log n) instead of O(n).

Worked intuition on `9 -3 4 4 0`:

```
pivot = median(9, 4, 0) = 4
partition → -3 0 [4] 4 9        (left ≤ 4 ≤ right)
recurse on -3 0 (insertion sort) and 4 9 (insertion sort)
result     -3 0 4 4 9
```

---

## What's in the implementation

| Function | Contract |
|---|---|
| `quicksort(v)` | Sorts `v` in place into non-decreasing order (public entry point) |
| `quicksort_range(v, lo, hi)` | Sorts inclusive range `v[lo..hi]` (internal) |
| `partition(v, lo, hi, pivot)` | Hoare split → `j` with `v[lo..j] ≤ pivot ≤ v[j+1..hi]` |
| `median_of_three(v, lo, hi)` | Median of the range's first, middle, last elements |
| `insertion_sort_range(v, lo, hi)` | Finishes ranges of ≤ 16 elements in place |

Details that matter in real code:

- **Median-of-three pivot** — single-pivot quicksort collapses on
  already-sorted input; sampling three endpoints keeps the split balanced for
  sorted, reversed, and random data alike.
- **Hoare partition with `lo ≤ j < hi`** — both sides always shrink, so the
  recursion terminates even when all keys are equal (40 identical elements is
  an explicit test case).
- **Recurse-small / loop-large** — only the smaller half goes on the stack;
  worst-case depth is O(log n) even if partitions skew.
- **Insertion cutoff of 16** — constant-factor win on tiny subarrays, and it
  bounds how much partitioning overhead the top levels can impose.

---

## Complexity

| Aspect | Cost |
|---|---|
| Time (expected / average) | O(n log n) |
| Time (worst case, pathological pivots) | O(n²) — rare with median-of-three |
| Extra space | O(log n) stack on average; in-place swaps otherwise |
| Stability | Not stable (swaps cross ranges) |

Empirically the worst case needs an adversarial pattern that defeats the
three-sample pivot; the 200 randomized test arrays (sizes 0–400, values
±1000) plus the low-cardinality duplicate stress all sort in O(n log n)
expected time.

---

## Build and run

```bash
g++ -std=c++17 -O2 -Wall -Wextra -o quicksort quicksort.cpp
./quicksort
```

Expected output (tests are compiled in — the binary fails loudly via `assert`
if any behavior regresses):

```
All quicksort tests passed.

before: 9 -3 4 4 0 7 -3 12 1 1 5 8 2 6 11 3 10 0 -7 13
after:  -7 -3 -3 0 0 1 1 2 3 4 4 5 6 7 8 9 10 11 12 13
```

---

## Test coverage

`run_tests()` asserts, among others:

- empty vector → unchanged; single element → unchanged
- two elements, distinct and equal (partition boundary cases) → match
  `std::sort`
- fully sorted 20-element run and fully reversed 20-element run → match
  the reference (median-of-three does its job)
- duplicates and negatives → reference equality
- 40 identical elements (larger than the insertion cutoff) → reference
  equality
- 200 fixed-seed random arrays, sizes 0–400 straddling the cutoff → reference
  equality
- 20 low-cardinality arrays (values 0–3, length 250) → reference equality,
  stressing duplicate-heavy partitions

---

## When to use it

**Good for:** general-purpose in-memory sorting, cache-friendly fast
averaging, sorting when you don't know the input distribution, and
divide-and-conquer patterns where partitioning is the natural split (quickselect
for k-th order statistics rides on the same machinery).

**Bad for:** guaranteed-O(n log n) requirements (use heapsort/mergesort or
`std::sort`'s introsort fallback), stable ordering of equal keys (not stable),
linked lists (no random access to pivot/swap), and extremely adversarial or
already-structured inputs unless the pivot defence above is kept intact.

---

## Series

Part of the **Algorithms in C++** repository — each lesson is a
self-contained folder with its implementation, its tests, and its write-up.
