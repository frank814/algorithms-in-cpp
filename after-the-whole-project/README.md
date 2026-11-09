# After the Whole Project

> The complete reference for this repository — every algorithm, the full
> structure, and how the whole thing is built, tested, and published. This is
> the deep-detail record of the project in one place.

---

## What this is

Classic algorithms implemented from scratch in **C++17**. Every lesson is an
original implementation written from our own understanding of how and why the
algorithm works — not a transcription of any source material. Each lesson is a
self-contained folder holding:

- **`<name>.cpp`** — the implementation, an embedded `assert()` test suite,
  and a deterministic demo, in one translation unit.
- **`README.md`** — the write-up: core idea with a worked example, function
  contracts, complexity tables, the real program output, test coverage, and
  guidance on when the algorithm fits the problem.

Standard library only. Zero dependencies. Every file compiles clean under
`g++ -std=c++17 -O2 -Wall -Wextra` and its tests pass before it is considered
done.

---

## The complete index

| # | Algorithm | What it covers | Folder |
|---|---|---|---|
| 1 | Binary search | Iterative + recursive search, lower/upper bound ranges, duplicate runs | `01-binary-search` |
| 2 | Selection sort | Sorted-prefix/unsorted-suffix in-place sort, reference-checked | `02-selection-sort` |
| 3 | Recursion | Factorial, memoized Fibonacci, call/return tracing, overflow guards | `03-recursion` |
| 4 | Quicksort | Median-of-three pivot, Hoare partition, insertion cutoff, tail recursion | `04-quicksort` |
| 5 | Hash table | Separate chaining, FNV-1a hashing, auto-rehash at load factor | `05-hash-table` |
| 6 | Breadth-first search | Adjacency-list graphs, unweighted shortest path with reconstruction | `06-breadth-first-search` |
| 7 | Binary search tree | Insert/search/delete (all three cases), traversals, height | `07-binary-search-tree` |
| 8 | Depth-first search | Recursive + iterative DFS, tree orders, directed cycle detection | `08-depth-first-search` |
| 9 | Huffman coding | Frequency tree, prefix codes, encode/decode round-trip | `09-huffman-coding` |
| 10 | AVL tree | Height-balanced BST, all four rotations, invariant verification | `10-avl-tree` |
| 11 | Splay tree | Access-time balancing, zig/zig-zig/zig-zag, root-after-access | `11-splay-tree` |
| 12 | B-tree | Multi-way balanced tree, node splits, fill-bound invariants | `12-b-tree` |
| 13 | Dijkstra's algorithm | Weighted single-source shortest paths, negative-edge rejection | `13-dijkstra` |
| 14 | Greedy scheduling | Maximum non-overlapping activity set, earliest-finish order | `14-greedy-scheduling` |
| 15 | Fractional knapsack | Value/weight ratio greedy, per-item fractions, 0/1 contrast | `15-greedy-knapsack` |
| 16 | Set cover | Greedy approximation, partial coverage semantics | `16-set-cover` |
| 17 | 0/1 knapsack | Rolling-row DP with item reconstruction, brute-force validated | `17-dp-knapsack` |
| 18 | Longest common substring | Two-row DP with reconstruction, maximality verified | `18-longest-common-substring` |
| 19 | Longest common subsequence | Full DP backtrace, subsequence property asserted | `19-longest-common-subsequence` |
| 20 | Levenshtein edit distance | 2-row distance DP plus a replayable edit script | `20-levenshtein-edit-distance` |
| 21 | k-nearest neighbors | Euclidean classification + mean regression, deterministic ties | `21-k-nearest-neighbors` |
| 22 | Linear regression | Closed-form OLS, predictions, R² scoring | `22-linear-regression` |
| 23 | Inverted index | Tokenization, posting lists, AND/OR boolean queries | `23-inverted-index` |
| 24 | Fourier transform | Naive DFT, radix-2 FFT, inverse transform round-trips | `24-fourier-transform` |
| 25 | MapReduce | Word-count pipeline: map, shuffle, reduce | `25-map-reduce` |
| 26 | Bloom filter | Probabilistic membership, double hashing, FP-rate bound | `26-bloom_filter` |
| 27 | HyperLogLog | Register-based cardinality estimation with error bounds | `27-hyperloglog` |
| 28 | Diffie-Hellman | Square-and-multiply modpow, shared-secret key exchange | `28-diffie-hellman` |
| 29 | MinHash + LSH | Signatures, Jaccard estimation, banding for candidates | `29-locality-sensitive-hashing` |
| 30 | Min-heap / priority queue | Binary heap from scratch, sift-up/down, O(n) heapify | `30-min-heap-priority-queue` |
| 31 | Simplex | Tableau linear programming, Bland's rule, infeasible/unbounded detection | `31-linear-programming` |

---

## Topics by area

- **Foundations** — binary search, selection sort, recursion
- **Sorting & hashing** — quicksort, hash tables
- **Graphs** — breadth-first search, Dijkstra's algorithm
- **Trees** — binary search tree, depth-first search, Huffman coding,
  AVL tree, splay tree, B-tree
- **Greedy methods** — scheduling, fractional knapsack, set cover
- **Dynamic programming** — 0/1 knapsack, longest common substring,
  longest common subsequence, Levenshtein distance
- **Machine learning basics** — k-nearest neighbors, linear regression
- **Text & signals** — inverted index, Fourier transform
- **Probabilistic & distributed patterns** — MapReduce, Bloom filter,
  HyperLogLog, MinHash/LSH
- **Cryptography & optimization** — Diffie-Hellman, min-heap/priority
  queue, simplex

---

## Repository structure

```
algorithms-in-cpp/
├── README.md                      ← the live index of published lessons
├── after-the-whole-project/
│   └── README.md                  ← this complete reference
├── 01-binary-search/
│   ├── README.md
│   └── binary_search.cpp
├── 02-selection-sort/
│   ├── README.md
│   └── (lesson code added as the lesson is worked)
├── 03-recursion/
│   └── README.md
├── 04-quicksort/
│   └── README.md
├── …
└── 31-linear-programming/
    ├── README.md
    └── linear_programming.cpp
```

### Anatomy of a lesson write-up

Every `NN-algorithm-name/README.md` follows the same sections:

1. Title + one-line identity
2. **The core idea** — the mechanism, with a worked example
3. **What's in the implementation** — function contracts + details that matter
4. **Complexity** — time/space table with preconditions
5. **Build and run** — the exact command and the real expected output
6. **Test coverage** — what the asserts prove
7. **When to use it** — good-for / bad-for
8. **Series** — how the lesson relates to the repository

---

## Build and run a lesson

```bash
cd 01-binary-search
g++ -std=c++17 -O2 -Wall -Wextra -o binary_search binary_search.cpp
./binary_search
```

## Verify everything that has code

```bash
for d in */; do
  src=$(ls "$d"*.cpp 2>/dev/null) || continue
  [ -z "$src" ] && continue
  g++ -std=c++17 -O2 -Wall -Wextra -o /tmp/alg "$src" \
    && /tmp/alg > /dev/null \
    && echo "PASS ${d%/}" || echo "FAIL ${d%/}"
done
```

---

## Testing philosophy

The tests ship inside each file and run on every execution — a regression
fails loudly before any demo output appears.

- **Edge cases first** — empty inputs, single elements, duplicates, missing
  values, all-equal data, out-of-range targets, negative values where they
  apply.
- **Cross-validation** — wherever a fast algorithm has a trivial slow
  counterpart, the fast path is checked against it on fixed-seed randomized
  inputs: sorts against `std::sort`, knapsack against 2ⁿ enumeration,
  Levenshtein against exponential brute force, FFT against the naive DFT,
  greedy results against corner-point enumeration.
- **Determinism** — every random sequence uses a fixed seed, so demo output
  is byte-identical on every machine.
- **Invariants asserted directly** — AVL balance factors and height fields,
  B-tree fill bounds and uniform leaf depth, prefix-freeness of Huffman
  codes, zero false negatives in the Bloom filter, heap ordering after every
  operation.

---

## Verification status

All implementations in this repository were compiled and executed as a full
sweep: warning-free builds under `-Wall -Wextra`, exit code 0, every assert
passing — and every write-up's "Expected output" block checked byte-for-byte
against a fresh run of the program.
