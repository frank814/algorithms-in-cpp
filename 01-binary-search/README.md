# 01 · Binary Search

> Algorithm **1 of 31** in the *Algorithms in C++* series — an original,
> from-scratch implementation with a full test suite. C++17, zero dependencies.

Binary search finds a target inside a **sorted** sequence by repeatedly cutting
the search space in half. Instead of checking elements one by one, it looks at
the middle, decides which half can still contain the answer, and discards the
other half. Each step throws away ~50% of what is left, so even a **billion**
sorted elements need at most **30 comparisons**.

---

## The core idea

Keep one rule (the *invariant*) true from start to finish:

> The target, if it exists, always lies inside `[lo, hi]`.

1. Look at the middle of the range.
2. Exact match → done.
3. Too small → everything up to and including the middle is impossible;
   search `(mid+1 … hi)`.
4. Too big → everything from the middle on is impossible; search `(lo … mid-1)`.
5. When the range collapses (`lo > hi`), the value is not there.

Worked example on `1 3 5 5 5 9 12`, hunting for `4`:

```
range [0..6]  mid=3  v=5   5 > 4  → discard right half
range [0..2]  mid=1  v=3   3 < 4  → discard left half
range [2..2]  mid=2  v=5   5 > 4  → discard right half
range [3..2]  empty         → not found, return -1
```

Three comparisons instead of seven. That gap explodes with size: at one
billion elements, linear scanning averages ~500 million comparisons; binary
search never exceeds 30.

---

## What's in the implementation

| Function | Contract |
|---|---|
| `binary_search_iter(v, target)` | Iterative search → index of a match, or `-1` |
| `binary_search_rec(v, target)` | Recursive search, same contract (O(log n) call stack) |
| `lower_bound_idx(v, target)` | First index with `v[i] >= target` (first occurrence / insertion point) |
| `upper_bound_idx(v, target)` | First index with `v[i] > target` (just past the last occurrence) |

Details that matter in real code:

- **Overflow-safe midpoint** — `lo + (hi - lo) / 2` instead of
  `(lo + hi) / 2`, which can overflow on huge ranges.
- **Half-open bounds for the range variants** — `lower_bound` /
  `upper_bound` use `[lo, hi)` with `hi = size()`, so "past the end" is a
  natural answer instead of a special case.
- **Duplicates handled deliberately** — a plain search may return *any*
  occurrence; the two range variants pin down the exact first/last positions
  (equivalently: count of a value = `upper - lower`).

---

## Complexity

| Variant | Time (avg / worst) | Extra space |
|---|---|---|
| Iterative | O(log n) / O(log n) | O(1) |
| Recursive | O(log n) / O(log n) | O(log n) stack |
| `lower_bound_idx` | O(log n) / O(log n) | O(1) |
| `upper_bound_idx` | O(log n) / O(log n) | O(1) |

The precondition for every variant: **the vector must be sorted ascending**
(`std::sort` / `std::is_sorted`). Feed it unsorted data and the halving logic
is meaningless — the one and only rule.

---

## Build and run

```bash
g++ -std=c++17 -O2 -Wall -Wextra -o binary_search binary_search.cpp
./binary_search
```

Expected output (tests are compiled in — the binary fails loudly via `assert`
if any behavior regresses):

```
All binary search tests passed.

Sorted data: 1 3 5 5 5 9 12
target=5  iter=3  rec=3  lower=2  upper=5
target=12  iter=6  rec=6  lower=6  upper=7
target=4  iter=-1  rec=-1  lower=2  upper=2
target=100  iter=-1  rec=-1  lower=7  upper=7
empty lower_bound(5)=0
```

---

## Test coverage

`run_tests()` asserts, among others:

- empty vector → `-1` / bounds return `0`
- single element → hit, miss below, miss above
- value below/above the whole range → `-1`
- absent interior value → `-1` (iterative and recursive agree)
- duplicates → `lower`/`upper` isolate the exact run (`lower(5)=2`, `upper(5)=5`)
- absent value → both bounds coincide at the insertion point
- all elements equal → any index is a valid hit; bounds span the whole vector
- iterative and recursive implementations agree everywhere

---

## When to use it

**Good for:** sorted arrays, sorted database keys / indexes, "first element
≥ x" range queries, existence checks in log time, any divide-and-conquer
search over an ordered domain.

**Bad for:** unsorted data (sort first — that's O(n log n), paid once),
linked lists (no O(1) random access), or data that changes between searches
unless you keep it re-sorted.

---

## Series

Part of the **Algorithms in C++** repository — 31 algorithms released one
lesson at a time, each in its own self-contained folder with tests. The
repository root `README.md` tracks the published lessons as they land.
