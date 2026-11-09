# Algorithms in C++

Algorithms in C++, built from scratch and tested — learning algorithms by building them.

## Files

### Binary Search — `binary_search.cpp`

Finds a target in a sorted list by cutting the search range in half on every step.

- Check the middle — match → return it; too small → keep the right half; too big → keep the left half.
- Empty range → not found (`-1`).
- Handles duplicated values with first / just-past-last positions.

### Selection Sort — `selection_sort.cpp`

Sorts a list in place by repeatedly picking the smallest remaining value.

- A sorted prefix grows at the front while the rest stays unsorted.
- Each pass scans for the minimum and swaps it into the next open slot.
- After n passes the list is sorted.

## Run

```bash
g++ -std=c++17 -O2 -Wall -Wextra -o selection_sort selection_sort.cpp
./selection_sort
```

## Verify every file

```bash
for f in *.cpp; do
  g++ -std=c++17 -O2 -Wall -Wextra -o /tmp/alg "$f" \
    && /tmp/alg > /dev/null \
    && echo "PASS $f" || echo "FAIL $f"
done
```
