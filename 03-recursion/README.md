# 03 · Recursion

> Algorithm **3** of the *Algorithms in C++* series — an original,
> from-scratch implementation with a full test suite. C++17, zero dependencies.

Recursion means a function solves a problem by calling itself on a smaller
piece of the same problem, until it reaches a base case small enough to answer
directly. This folder works the pattern through two classics — factorial and
Fibonacci — pairing each recursive version with an iterative reference so the
results can be cross-checked, and adding a call/return trace that makes the
unwinding visible.

---

## The core idea

Every recursive function needs two things:

> 1. A **base case** that returns without recursing.
> 2. A **recursive step** that moves strictly closer to that base case.

Factorial illustrates it directly: `n! = n × (n-1)!` with `0! = 1! = 1`.

```
factorial_rec(4)
  → 4 × factorial_rec(3)
      → 3 × factorial_rec(2)
          → 2 × factorial_rec(1)
              → 1                (base case)
          ← 2
      ← 6
  ← 24
```

Each pending multiplication waits on the stack until the deepest call returns,
then the frames unwind one level at a time. The second classic, Fibonacci
(`F(n) = F(n-1) + F(n-2)`), shows the danger: without help it recomputes the
same subproblems exponentially often. A memo cache (each value computed once)
turns that into a single linear sweep — the seed of dynamic programming.

---

## What's in the implementation

| Function | Contract |
|---|---|
| `factorial_iter(n)` | Iterative reference → `n!` as `uint64_t`; caller guarantees `n ≤ 20` |
| `factorial_rec(n)` | Recursive `n!`; returns `0` as an overflow guard for `n > 20` |
| `fib_iter(n)` | Iterative reference → `F(n)` (`F(0)=0`, `F(1)=1`) |
| `fib_memo(n, cache)` | Recursive Fibonacci filling a caller-supplied memo vector |
| `fib_memo(n)` | Convenience wrapper owning a cache sized for `0 … n` |
| `trace_factorial(n)` | Prints an indented call/return trace, one level per two spaces |

Details that matter in real code:

- **Overflow guard instead of silent wraparound** — `20! = 2432902008176640000`
  fits a 64-bit unsigned integer, `21!` does not, so `factorial_rec` refuses
  (`returns 0`) rather than producing garbage. The iterative twin documents
  the same `n ≤ 20` precondition.
- **Memo sentinel of 0** — an uncomputed cache slot holds `0`, and `F(0) = 0`
  is seeded explicitly so the sentinel never collides with a real answer.
- **Depth is the space cost** — each pending call keeps a live stack frame;
  that is why recursive `factorial` uses O(n) stack while the loop uses O(1).

---

## Complexity

| Function | Time | Extra space |
|---|---|---|
| `factorial_iter` | O(n) | O(1) |
| `factorial_rec` | O(n) | O(n) stack frames |
| `fib_iter` | O(n) | O(1) |
| `fib_memo` (with cache) | O(n) — each `i` computed once | O(n) cache + O(n) stack |
| plain recursive Fibonacci (not shipped) | O(2ⁿ) | O(n) stack |
| `trace_factorial` | O(n) | O(n) stack + O(n) output |

Preconditions: inputs are `unsigned`; factorial results are only valid through
`n = 20` (beyond that the guard returns `0`), and Fibonacci values must fit a
`uint64_t` (`F(93)` is the last one that does).

---

## Build and run

```bash
g++ -std=c++17 -O2 -Wall -Wextra -o recursion recursion.cpp
./recursion
```

Expected output (tests are compiled in — the binary fails loudly via `assert`
if any behavior regresses):

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

---

## Test coverage

`run_tests()` asserts, among others:

- `factorial(0) = 1`, `factorial(1) = 1`, `factorial(2) = 2`,
  `factorial(10) = 3628800` in both styles
- recursive and iterative factorial agree for every `n` in `0 … 20`
- overflow guard fires: `factorial_rec(21) = 0`, `factorial_rec(100) = 0`
- `fib(0) = 0`, `fib(1) = 1`, `fib(2) = 1`, `fib(20) = 6765`,
  `fib(50) = 12586269025`
- memoized and iterative Fibonacci agree for every `n` in `0 … 60`
- repeated calls through a shared cache return identical values
  (`fib_memo(10)` twice, then `fib_memo(50)`)

---

## When to use it

**Good for:** problems with naturally nested structure — trees, graphs,
parentheses/AST traversal, divide-and-conquer — plus anything where the
recursive definition *is* the specification (factorial, Fibonacci, Catalan
numbers). Memoization makes repeated-subproblem recursion cheap.

**Bad for:** flat loops that a compiler can optimize better than the call
stack; deep chains where frames blow the stack (tens of thousands of levels);
naive double recursion on large inputs without a cache; and any recursion
missing a decreasing measure, which turns into infinite descent.

---

## Series

Part of the **Algorithms in C++** repository — each lesson is a
self-contained folder with its implementation, its tests, and its write-up.
