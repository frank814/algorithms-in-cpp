# 01 · Binary Search

Finds a target in a **sorted** list by cutting the search range in half on every step — O(log n).

## How it works

- Check the middle of the range.
- Match → return the index. Too small → keep the right half. Too big → keep the left half.
- Range empty → not found (`-1`).
- The file also includes `lower_bound_idx` / `upper_bound_idx` for duplicated values (first position, and just past the last).

## Run

```bash
g++ -std=c++17 -O2 -Wall -Wextra -o binary_search binary_search.cpp
./binary_search
```

Expected:

```
All binary search tests passed.

Sorted data: 1 3 5 5 5 9 12
target=5  iter=3  rec=3  lower=2  upper=5
target=12  iter=6  rec=6  lower=6  upper=7
target=4  iter=-1  rec=-1  lower=2  upper=2
target=100  iter=-1  rec=-1  lower=7  upper=7
empty lower_bound(5)=0
```
