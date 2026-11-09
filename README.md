# Algorithms in C++

Classic algorithms implemented from scratch in **C++17** — every file carries its own `assert()` tests. Standard library only, zero dependencies.

## Files

### 01 · Binary Search — `binary_search.cpp`

Finds a target in a **sorted** list by cutting the search range in half on every step (O(log n)).

- Check the middle — match → return the index; too small → keep the right half; too big → keep the left half.
- Empty range → not found (`-1`).
- Includes first / just-past-last position helpers for duplicated values.

## Run

```bash
g++ -std=c++17 -O2 -Wall -Wextra -o binary_search binary_search.cpp
./binary_search
```

## Verify every file

```bash
for f in *.cpp; do
  g++ -std=c++17 -O2 -Wall -Wextra -o /tmp/alg "$f" \
    && /tmp/alg > /dev/null \
    && echo "PASS $f" || echo "FAIL $f"
done
```
