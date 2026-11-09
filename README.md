# Algorithms in C++

> **31 algorithms, written from scratch in C++17 — published one lesson at a
> time.** Each update ships exactly one algorithm: its implementation, its
> tests, and its full write-up. Nothing is listed here before it exists.

Every algorithm lives in its own self-contained folder — one `.cpp` that
compiles with a single command, carries a real `assert()` test suite, and runs
a deterministic demo. Standard library only, zero dependencies. The code and
explanations are original: written from our own understanding of how and why
each algorithm works, with the tough cases (duplicates, empty inputs,
degenerate structures, numerical edge cases) handled on purpose.

---

## Published lessons

**Progress: 1 / 31**

| # | Algorithm | Lesson |
|---|---|---|
| 1 | Binary search (+ lower/upper bound) | [`01-binary-search`](01-binary-search/) |

The next lesson is added here as soon as it is completed — one algorithm per
update, never a bulk dump.

---

## Layout of a lesson

```
NN-algorithm-name/
├── README.md        ← the full write-up (idea, complexity, real output, tests)
└── algorithm_name.cpp   ← implementation + test suite + demo in one file
```

---

## Build and run a lesson

```bash
cd 01-binary-search
g++ -std=c++17 -O2 -Wall -Wextra -o binary_search binary_search.cpp
./binary_search
```

## Verify every published lesson

```bash
for d in */; do
  src=$(ls "$d"*.cpp)
  g++ -std=c++17 -O2 -Wall -Wextra -o /tmp/alg "$src" \
    && /tmp/alg > /dev/null \
    && echo "PASS ${d%/}" || echo "FAIL ${d%/}"
done
```

Expected: `PASS` for every published folder.

---

## Testing philosophy

Each file ships with its tests (they run on every execution — a regression
fails loudly before any output):

- **Edge cases first** — empty inputs, single elements, duplicates, missing
  values, all-equal data, out-of-range targets.
- **Cross-validation** — whenever a fast algorithm has a trivial slow
  counterpart (sorting vs `std::sort`, knapsack vs 2ⁿ enumeration, FFT vs
  DFT), the fast path is checked against it on fixed-seed randomized inputs.
- **Determinism** — all randomness uses fixed seeds; demo output is identical
  on every machine.
- **Invariants** — structural properties are asserted directly (balance
  factors, tree fill bounds, prefix-freeness, zero false negatives).

---

## The series

Topics, in order: searching · sorting · recursion · hashing · graphs · trees ·
greedy methods · dynamic programming · machine learning · text processing ·
signal processing · probabilistic data structures · cryptography ·
optimization — each published here as its lesson is completed, until all 31
stand together with the complete index.
