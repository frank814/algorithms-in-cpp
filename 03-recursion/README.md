# 03 · Recursion

A function solves smaller copies of the same problem until a base case answers directly — shown here with factorial and Fibonacci.

## How it works

- Base case: return without recursing. Recursive step: move closer to the base case.
- Each waiting call sits on the call stack until the deepest one returns, then the frames unwind.
- Plain recursive Fibonacci recomputes the same values over and over — a memo cache computes each value once (O(n)).

## Run

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
