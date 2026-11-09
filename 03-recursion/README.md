# 03 · Recursion — Run

```bash
g++ -std=c++17 -O2 -Wall -Wextra -o recursion recursion.cpp
./recursion
```

Expected:

```
All recursion tests passed.

factorial_rec(10) = 3628800
factorial_rec(20) = 2432902008176640000
factorial_rec(21) = 0  (overflow guard)
fib_memo(20)      = 6765
fib_memo(50)      = 12586269025
fib_iter(50)      = 12586269025  (matches memoized)

trace_factorial(4):
call  factorial(4)
  call  factorial(3)
    call  factorial(2)
      call  factorial(1)
      return factorial(1) -> 1
    return factorial(2) -> 2
  return factorial(3) -> 6
return factorial(4) -> 24
```
