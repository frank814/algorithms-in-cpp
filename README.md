# Algorithms in C++

Classic algorithms implemented from scratch in **C++17**, each in its own
self-contained folder: one source file containing the implementation, a real
`assert()` test suite, and a deterministic demo. Standard library only —
zero dependencies.

The code and explanations are original: written from our own understanding
of how and why each algorithm works, with the tough cases (duplicates, empty
inputs, degenerate structures, numerical edge cases) handled on purpose.

---

## Lessons

| # | Algorithm | Lesson |
|---|---|---|
| 1 | Binary search (+ lower/upper bound) | [`01-binary-search`](01-binary-search/) |

---

## Layout of a lesson

```
NN-algorithm-name/
├── README.md            ← the write-up (idea, complexity, real output, tests)
└── algorithm_name.cpp   ← implementation + test suite + demo in one file
```

---

## Build and run

```bash
cd 01-binary-search
g++ -std=c++17 -O2 -Wall -Wextra -o binary_search binary_search.cpp
./binary_search
```

## Verify every lesson

```bash
for d in */; do
  src=$(ls "$d"*.cpp)
  g++ -std=c++17 -O2 -Wall -Wextra -o /tmp/alg "$src" \
    && /tmp/alg > /dev/null \
    && echo "PASS ${d%/}" || echo "FAIL ${d%/}"
done
```

Expected: `PASS` for every folder.

---

## Testing philosophy

Every file ships with its tests (they run on every execution — a regression
fails loudly before any output):

- **Edge cases first** — empty inputs, single elements, duplicates, missing
  values, all-equal data, out-of-range targets.
- **Cross-validation** — wherever a fast algorithm has a trivial slow
  counterpart (sorting vs `std::sort`, knapsack vs 2ⁿ enumeration, FFT vs
  DFT), the fast path is checked against it on fixed-seed randomized inputs.
- **Determinism** — all randomness uses fixed seeds; demo output is identical
  on every machine.
- **Invariants** — structural properties are asserted directly (balance
  factors, tree fill bounds, prefix-freeness, zero false negatives).
