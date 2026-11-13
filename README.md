# Algorithms in C++

Every algorithm here is written from scratch and tested — built for learning how they actually work.

All you need is a C++ compiler (g++, clang++, or MSVC) — no libraries, no setup.

## Files

### [Binary Search](binary_search.cpp)

Finds a target in a sorted list by cutting the search range in half on every step.

- Keeps two markers, `lo` and `hi`, around the part of the list still worth searching.
- Looks at the middle element: it is the target → done; the target is smaller → drop the whole right half; bigger → drop the whole left half.
- Repeats until the middle hits or the range becomes empty (not found → `-1`).
- Two extra helpers return the first or just-past-the-last position of a value, so duplicate-heavy lists behave predictably.

### [Selection Sort](selection_sort.cpp)

Sorts a list in place by repeatedly picking the smallest remaining value.

- The left part of the list is the *sorted prefix*; everything to its right is still unsorted.
- Each pass scans the unsorted part for its minimum value and swaps it into the first open slot.
- The prefix grows by one element per pass until the whole list is sorted.
- Needs no extra memory — the swaps happen inside the original list.

### [Recursion](recursion.cpp)

A function solves smaller copies of the same problem until a base case answers directly.

- Every call that cannot answer yet waits on the call stack; the deepest call (the base case) answers first.
- The waiting calls then unwind one frame at a time, each finishing its own piece of the answer.
- `factorial` guards against 64-bit overflow by returning `0` for `n > 20`.
- `fibonacci` keeps a small memo table, so each value is computed once instead of being recomputed down every branch.

### [Quicksort](quicksort.cpp)

Sorts in place by splitting around a pivot and sorting the two sides.

- The pivot is chosen as the median of the first, middle, and last elements — a balanced pick keeps the splits even.
- The partition step walks two pointers inward, swapping anything on the wrong side of the pivot, and finishes with the pivot in its final correct position.
- After one partition, everything left of the pivot is smaller and everything right is larger — so those two halves never interfere again.
- Recurses on both halves, stopping at empty or single-element ranges; a guard never lets it recurse over an unchanged whole array.

### [Hash Table](hash_table.cpp)

Keeps key-to-value pairs so a value can be fetched by its key in about the same time whether the table holds ten entries or ten million.

- A key is hashed (64-bit FNV-1a, provided for strings and integers) straight to a bucket number.
- Separate chaining: each bucket owns a chain of entries, and a lookup scans only that one chain.
- When the load factor passes 0.75 the table automatically rehashes into roughly double the buckets, which keeps chains short.
- Average insert, get, contains, and erase are O(1); the worst case, every key colliding into one chain, is O(n).

### [Breadth-First Search](bfs.cpp)

Searches an unweighted graph layer by layer and returns the shortest path from a start node to a target.

- The graph stores each node's neighbor list, built either directed or undirected.
- A FIFO queue explores every node one step away first, then two steps away, and so on, so distance grows in clean layers.
- The first path that reaches the target is the shortest one, because every edge costs exactly the same.
- Unreachable targets come back as distance -1, the source alone is distance 0, and the whole search runs in O(V + E).

### [Binary Search Tree](binary_search_tree.cpp)

Stores keys in a tree where everything smaller sits to the left and everything larger to the right, so insert, search, and delete follow one branch at a time.

- Insert and search walk down comparing keys, left when smaller and right when bigger; duplicates are ignored, so every key lives in exactly one place.
- Deleting a node with two children swaps in its in-order successor, the smallest key on the right, which keeps the ordering intact everywhere else.
- In-order traversal reads the tree out already sorted, and height measures the deepest branch.
- Average operations take O(log n); a tree skewed by sorted inserts degrades to O(n), the honest worst case.

## Run

One example — the same two commands run any file in this repo:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -o run quicksort.cpp && ./run
```

What you should see:

```
All quicksort tests passed.

before: 9 -3 4 4 0 7 -3 12 1 1 5 8 2 6 11 3 10 0 -7 13
after:  -7 -3 -3 0 0 1 1 2 3 4 4 5 6 7 8 9 10 11 12 13
```

Or run every file in one go:

```bash
for f in *.cpp; do
  g++ -std=c++17 -O2 -Wall -Wextra -o /tmp/alg "$f" \
    && /tmp/alg > /dev/null \
    && echo "PASS $f" || echo "FAIL $f"
done
```

Which prints:

```
PASS bfs.cpp
PASS binary_search.cpp
PASS binary_search_tree.cpp
PASS hash_table.cpp
PASS quicksort.cpp
PASS recursion.cpp
PASS selection_sort.cpp
```
