# Algorithms in C++

Classic algorithms implemented from scratch in **C++17** — one folder per algorithm: a single source file with its tests, plus a short write-up. Standard library only, zero dependencies.

## Lessons

| # | Algorithm | Lesson |
|---|---|---|
| 1 | Binary search | [`01-binary-search`](01-binary-search/) |
| 2 | Selection sort | [`02-selection-sort`](02-selection-sort/) |
| 3 | Recursion | [`03-recursion`](03-recursion/) |
| 4 | Quicksort | [`04-quicksort`](04-quicksort/) |

## Run a lesson

```bash
cd 01-binary-search
g++ -std=c++17 -O2 -Wall -Wextra -o binary_search binary_search.cpp
./binary_search
```

## Verify everything

```bash
for d in */; do
  src=$(ls "$d"*.cpp 2>/dev/null) || continue
  [ -z "$src" ] && continue
  g++ -std=c++17 -O2 -Wall -Wextra -o /tmp/alg "$src" \
    && /tmp/alg > /dev/null \
    && echo "PASS ${d%/}" || echo "FAIL ${d%/}"
done
```
