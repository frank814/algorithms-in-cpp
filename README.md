# Algorithms in C++

> **31 algorithms, written from scratch in C++17 — every one compiled,
> tested, and proven before it landed here.**

A hands-on algorithms library built for learning by doing: each algorithm
lives in its own self-contained folder with a single source file that contains
the implementation, a real `assert()` test suite, and a deterministic demo.
Nothing is a stub or a pseudocode sketch — every file compiles warning-free
and its tests pass before it is added to the series.

The implementations and explanations are original: written from our own
understanding of how and why each algorithm works, with the tough cases
(duplicates, empty inputs, degenerate structures, numerical edge cases)
handled on purpose.

---

## Repository layout

```
algorithms-in-cpp/
├── README.md                 ← this index
├── 01-binary-search/
│   ├── README.md             ← full write-up for algorithm 1
│   └── binary_search.cpp
├── 02-selection-sort/
│   └── selection_sort.cpp
├── ...
└── 31-linear-programming/
    └── linear_programming.cpp
```

Every folder is **standalone**: one `.cpp`, compiles with a single command,
no dependencies beyond the C++17 standard library. Each folder can become
its own GitHub repository without any restructuring.

---

## The full series

| # | Algorithm | Folder |
|---|---|---|
| 1 | Binary search (+ lower/upper bound) | [`01-binary-search`](01-binary-search/) |
| 2 | Selection sort | [`02-selection-sort`](02-selection-sort/) |
| 3 | Recursion — factorial, memoized Fibonacci, call traces | [`03-recursion`](03-recursion/) |
| 4 | Quicksort (median-of-three, in-place) | [`04-quicksort`](04-quicksort/) |
| 5 | Hash table (chaining, FNV-1a, auto-rehash) | [`05-hash-table`](05-hash-table/) |
| 6 | Breadth-first search (shortest path) | [`06-breadth-first-search`](06-breadth-first-search/) |
| 7 | Binary search tree (insert/search/delete) | [`07-binary-search-tree`](07-binary-search-tree/) |
| 8 | Depth-first search (traversals, cycle detection) | [`08-depth-first-search`](08-depth-first-search/) |
| 9 | Huffman coding (optimal prefix codes) | [`09-huffman-coding`](09-huffman-coding/) |
| 10 | AVL tree (self-balancing) | [`10-avl-tree`](10-avl-tree/) |
| 11 | Splay tree (self-rotating) | [`11-splay-tree`](11-splay-tree/) |
| 12 | B-tree (multi-way balanced) | [`12-b-tree`](12-b-tree/) |
| 13 | Dijkstra's algorithm (weighted shortest path) | [`13-dijkstra`](13-dijkstra/) |
| 14 | Greedy scheduling (max non-overlapping set) | [`14-greedy-scheduling`](14-greedy-scheduling/) |
| 15 | Fractional knapsack (ratio greedy) | [`15-greedy-knapsack`](15-greedy-knapsack/) |
| 16 | Set cover (greedy approximation) | [`16-set-cover`](16-set-cover/) |
| 17 | 0/1 knapsack (dynamic programming) | [`17-dp-knapsack`](17-dp-knapsack/) |
| 18 | Longest common substring (DP) | [`18-longest-common-substring`](18-longest-common-substring/) |
| 19 | Longest common subsequence (DP + backtrace) | [`19-longest-common-subsequence`](19-longest-common-subsequence/) |
| 20 | Levenshtein edit distance (+ edit script) | [`20-levenshtein-edit-distance`](20-levenshtein-edit-distance/) |
| 21 | k-nearest neighbors (classification + regression) | [`21-k-nearest-neighbors`](21-k-nearest-neighbors/) |
| 22 | Linear regression (OLS + R²) | [`22-linear-regression`](22-linear-regression/) |
| 23 | Inverted index (boolean search) | [`23-inverted-index`](23-inverted-index/) |
| 24 | Fourier transform (DFT + radix-2 FFT) | [`24-fourier-transform`](24-fourier-transform/) |
| 25 | MapReduce (word-count pipeline) | [`25-map-reduce`](25-map-reduce/) |
| 26 | Bloom filter (probabilistic membership) | [`26-bloom_filter`](26-bloom_filter/) |
| 27 | HyperLogLog (cardinality estimation) | [`27-hyperloglog`](27-hyperloglog/) |
| 28 | Diffie-Hellman key exchange | [`28-diffie-hellman`](28-diffie-hellman/) |
| 29 | MinHash + LSH (set similarity) | [`29-locality-sensitive-hashing`](29-locality-sensitive-hashing/) |
| 30 | Min-heap / priority queue | [`30-min-heap-priority-queue`](30-min-heap-priority-queue/) |
| 31 | Simplex (linear programming) | [`31-linear-programming`](31-linear-programming/) |

Topics covered: **searching · sorting · recursion · hashing · graphs · trees
(4 flavors) · greedy methods · dynamic programming · machine learning ·
text processing · signal processing · probabilistic data structures ·
cryptography · optimization.**

---

## Build and run one algorithm

```bash
cd 01-binary-search
g++ -std=c++17 -O2 -Wall -Wextra -o binary_search binary_search.cpp
./binary_search
```

## Verify the whole series

```bash
for d in */; do
  src=$(ls "$d"*.cpp)
  g++ -std=c++17 -O2 -Wall -Wextra -o /tmp/alg "$src" \
    && /tmp/alg > /dev/null \
    && echo "PASS ${d%/}" || echo "FAIL ${d%/}"
done
```

Expected: `PASS` for all 31 folders.

---

## Testing philosophy

Each file ships with its tests (they run on every execution — a regression
fails loudly before any output):

- **Edge cases first** — empty inputs, single elements, duplicates, missing
  values, all-equal data, out-of-range targets.
- **Cross-validation** — whenever a fast algorithm has a trivial slow
  counterpart (sorting vs `std::sort`, knapsack vs 2ⁿ enumeration, Levenshtein
  vs brute force, FFT vs DFT), the fast path is checked against it on
  fixed-seed randomized inputs.
- **Determinism** — all randomness uses fixed seeds; demo output is identical
  on every machine.
- **Invariants** — structural properties are asserted directly (AVL balance
  factors, B-tree fill bounds and leaf depths, prefix-freeness of Huffman
  codes, zero false negatives in the Bloom filter).

---

## How this series is released

Each algorithm is written, verified, and kept ready in its folder. Algorithms
are published one at a time — a folder (with its write-up) becomes a GitHub
repository (or a commit in a shared repo) exactly when its lesson is reviewed.
No bulk dumps, no untested code.
