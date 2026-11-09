// Recursion fundamentals: factorial and Fibonacci, each paired with an
// iterative reference so the recursive results can be verified.
//
// factorial_rec(n): guards n <= 20 because 20! = 2432902008176640000 fits a
//   64-bit unsigned integer while 21! overflows it.
//   Time O(n), recursion depth O(n).
// fib_memo(n): Fibonacci with a vector cache; each value computed once.
//   Time O(n), space O(n) for the cache.
// trace_factorial(n): prints a call/return trace with indented nesting depth.
//
// The iterative reference implementations run in O(n) time and O(1) space.

#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

// Iterative factorial reference. Caller guarantees n <= 20.
std::uint64_t factorial_iter(unsigned n) {
    std::uint64_t acc = 1;
    for (unsigned i = 2; i <= n; ++i) {
        acc *= i;
    }
    return acc;
}

// Recursive factorial; refuses to compute values that would overflow 64 bits.
std::uint64_t factorial_rec(unsigned n) {
    if (n > 20) {
        // 21! exceeds 2^64-1; bail out instead of silently overflowing.
        return 0;
    }
    if (n <= 1) {
        return 1;
    }
    return n * factorial_rec(n - 1);
}

// Iterative Fibonacci reference (F(0)=0, F(1)=1).
std::uint64_t fib_iter(unsigned n) {
    std::uint64_t prev = 0;
    std::uint64_t cur = 1;
    for (unsigned i = 0; i < n; ++i) {
        std::uint64_t next = prev + cur;
        prev = cur;
        cur = next;
    }
    return prev;
}

// Recursive Fibonacci with a memo table; cache[i] == 0 means "not computed"
// (0 is also the true value of F(0), but F(0) is seeded explicitly below).
std::uint64_t fib_memo(unsigned n, std::vector<std::uint64_t>& cache) {
    if (n < cache.size() && cache[n] != 0) {
        return cache[n];
    }
    if (n <= 1) {
        return n;
    }
    std::uint64_t value = fib_memo(n - 1, cache) + fib_memo(n - 2, cache);
    if (n < cache.size()) {
        cache[n] = value;
    }
    return value;
}

// Convenience wrapper owning the cache; sized to hold every value up to n.
std::uint64_t fib_memo(unsigned n) {
    std::vector<std::uint64_t> cache(n + 1, 0);
    cache[0] = 0;
    if (n >= 1) {
        cache[1] = 1;
    }
    return fib_memo(n, cache);
}

// Prints an indented call/return trace of factorial(n): one "call" line per
// recursion level, then one "return" line per level as each frame unwinds.
// Returns the computed value so the helper can recurse while staying honest.
static std::uint64_t trace_factorial_impl(unsigned n, unsigned depth) {
    const std::string indent(depth * 2, ' ');
    std::cout << indent << "call  factorial(" << n << ")\n";
    std::uint64_t result;
    if (n <= 1) {
        result = 1;
    } else {
        result = n * trace_factorial_impl(n - 1, depth + 1);
    }
    std::cout << indent << "return factorial(" << n << ") -> " << result
              << '\n';
    return result;
}

void trace_factorial(unsigned n) {
    trace_factorial_impl(n, 0);
}

static void run_tests() {
    // Required values.
    assert(factorial_rec(0) == 1);
    assert(factorial_iter(0) == 1);
    assert(factorial_rec(10) == 3628800);
    assert(factorial_iter(10) == 3628800);
    assert(fib_memo(20) == 6765);
    assert(fib_iter(20) == 6765);
    assert(fib_memo(50) == 12586269025ULL);
    assert(fib_iter(50) == 12586269025ULL);

    // Recursive factorial equals iterative factorial across the safe range.
    for (unsigned n = 0; n <= 20; ++n) {
        assert(factorial_rec(n) == factorial_iter(n));
    }
    // The guard returns 0 for values that would overflow.
    assert(factorial_rec(21) == 0);
    assert(factorial_rec(100) == 0);

    // Factorial edge cases.
    assert(factorial_rec(1) == 1);
    assert(factorial_rec(2) == 2);

    // Memoized Fibonacci equals iterative Fibonacci for n = 0..60.
    for (unsigned n = 0; n <= 60; ++n) {
        assert(fib_memo(n) == fib_iter(n));
    }

    // Fibonacci edge cases: F(0)=0, F(1)=1, F(2)=1.
    assert(fib_memo(0) == 0);
    assert(fib_memo(1) == 1);
    assert(fib_memo(2) == 1);
    assert(fib_iter(0) == 0);
    assert(fib_iter(1) == 1);

    // Repeated memo calls keep returning the same value (cache reuse).
    std::vector<std::uint64_t> cache(51, 0);
    cache[1] = 1;
    assert(fib_memo(10, cache) == 55);
    assert(fib_memo(10, cache) == 55);
    assert(fib_memo(50, cache) == 12586269025ULL);
}

int main() {
    run_tests();
    std::cout << "All recursion tests passed.\n\n";

    // Deterministic demo: verified values.
    std::cout << "factorial_rec(10) = " << factorial_rec(10) << '\n';
    std::cout << "factorial_rec(20) = " << factorial_rec(20) << '\n';
    std::cout << "factorial_rec(21) = " << factorial_rec(21)
              << "  (overflow guard)\n";
    std::cout << "fib_memo(20)      = " << fib_memo(20) << '\n';
    std::cout << "fib_memo(50)      = " << fib_memo(50) << '\n';
    std::cout << "fib_iter(50)      = " << fib_iter(50)
              << "  (matches memoized)\n\n";

    // Indented call/return trace.
    std::cout << "trace_factorial(4):\n";
    trace_factorial(4);
    return 0;
}
