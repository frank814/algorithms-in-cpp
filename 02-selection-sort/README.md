# 02 · Selection Sort

> Algorithm **2** of the *Algorithms in C++* series — an original,
> from-scratch implementation with a full test suite. C++17, zero dependencies.

Selection sort splits the array into a sorted prefix and an unsorted suffix.
Each round it scans the entire suffix for the smallest value still waiting and
swaps that value into the next open slot at the front. The prefix grows by one
element per pass until the whole vector is in non-decreasing order. Nothing
about the scan depends on what the data looks like — sorted, shuffled, or
backwards all take the same number of comparisons.

---

## The core idea

Maintain this invariant after pass `i`:

> Positions `0 … i-1` hold the `i` smallest elements seen so far, in order.

1. Start from position `i` with `min_pos = i`.
2. Sweep the rest of the vector (`i+1 … n-1`); whenever an element beats the
   current best, remember its index in `min_pos`.
3. If `min_pos` moved, swap it with position `i` — the minimum of the unsorted
   suffix just joined the sorted prefix.
4. Advance `i` and repeat. After `n` passes the whole vector is sorted.

Worked example on `5 3 0 3 -11` (showing only the swap that ends each pass):

```
pass 0  scan 3 0 3 -11 for min  → -11 at index 4  swap(0,4) → -11 3 0 3 5
pass 1  scan 0 3 5 for min      → 0 at index 2    swap(1,2) → -11 0 3 3 5
pass 2  scan 3 3 5 for min      → 3 at index 2    already there
pass 3  scan 3 5 for min        → 3 at index 3    already there
pass 4  last slot               → sorted
```

The cost is fixed: roughly `n + (n-1) + … + 1 ≈ n²/2` comparisons no matter
how favourable the input is. That predictability is the algorithm's
personality — never fast, never surprised.

---

## What's in the implementation

| Function | Contract |
|---|---|
| `selection_sort(v)` | Sorts `v` in place into non-decreasing order |
| `is_sorted_non_decreasing(v)` | `true` when every element is `<= ` its successor |
| `check_against_reference(v)` | Test helper: sorts a copy with `std::sort` and asserts equality |

Details that matter in real code:

- **Swap only when it helps** — if `min_pos == i` the minimum was already in
  place, so no write happens. Sorted input costs zero swaps.
- **Hand-rolled sortedness check** — `is_sorted_non_decreasing` verifies the
  property directly instead of trusting the sort, so the tests confirm the
  postcondition independently.
- **Reference-checked, not just "looks sorted"** — every test case compares
  against `std::sort` output element-for-element, which also pins down
  duplicate ordering (multiset equality) rather than only checking pairwise
  order.

---

## Complexity

| Aspect | Cost |
|---|---|
| Time (best) | O(n²) — the scan still runs even on sorted input |
| Time (average) | O(n²) |
| Time (worst) | O(n²) |
| Extra space | O(1) — fully in place |
| Swaps | at most `n - 1` (one per pass) |

Comparisons are `n(n-1)/2` regardless of data; swaps are few, which matters
when writing to a medium is far more expensive than reading from it.

---

## Build and run

```bash
g++ -std=c++17 -O2 -Wall -Wextra -o selection_sort selection_sort.cpp
./selection_sort
```

Expected output (tests are compiled in — the binary fails loudly via `assert`
if any behavior regresses):

```
All selection sort tests passed.

before: -5 3 0 3 -11 8
after:  -11 -5 0 3 3 8
```

---

## Test coverage

`run_tests()` asserts, among others:

- empty vector → stays empty
- single element → unchanged
- already-sorted input → matches the `std::sort` reference
- reverse-sorted input (including negatives) → matches the reference
- duplicates → multiset order matches the reference
- all elements equal → vector unchanged
- negatives mixed with positives → matches the reference
- fixed-seed random arrays of 200 and 37 elements → match the reference
- sortedness property re-checked with `is_sorted_non_decreasing` on random data

---

## When to use it

**Good for:** tiny arrays where simplicity beats speed, teaching the
sorted-prefix/unsorted-suffix pattern, situations where the *number of writes*
must be minimal (each pass does at most one swap), and as a baseline that
other sorts are measured against.

**Bad for:** anything large — O(n²) even in the best case; production code
should reach for `std::sort` (introsort) instead. Also a poor fit when you
need a *stable* sort: swapping distant elements can reorder equal keys, so
this variant guarantees no relative-order preservation.

---

## Series

Part of the **Algorithms in C++** repository — each lesson is a
self-contained folder with its implementation, its tests, and its write-up.
