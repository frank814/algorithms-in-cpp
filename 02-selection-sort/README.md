# 02 · Selection Sort

Sorts a list in place by repeatedly picking the smallest remaining value and swapping it into position — O(n²), no extra memory.

## How it works

- Keep a sorted prefix at the front; everything after it stays unsorted.
- Each pass scans the unsorted part for its minimum and swaps it to the next open slot.
- After n passes the whole list is sorted.

## Run

```bash
g++ -std=c++17 -O2 -Wall -Wextra -o selection_sort selection_sort.cpp
./selection_sort
```

Expected:

```
All selection sort tests passed.

before: -5 3 0 3 -11 8
after:  -11 -5 0 3 3 8
```
