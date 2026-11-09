# 01 · Binary Search — Run

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
