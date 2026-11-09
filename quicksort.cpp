// In-place quicksort with median-of-three pivot selection and an
// insertion-sort cutoff for small subranges.
//
// - Pivot: the median of the first, middle, and last element, which keeps
//   partitions balanced on already-sorted or reverse-sorted input.
// - Partition: Hoare's scheme, which degrades gracefully on all-equal input.
// - Ranges of <= k elements are finished with insertion sort (cheaper than
//   further partitioning), and the larger side is handled iteratively so the
//   recursion depth stays O(log n) in practice.
//
// Time: O(n log n) expected/average, O(n^2) pathological worst case.
// Space: O(log n) stack on average, in-place otherwise.

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <vector>

namespace {

constexpr int kInsertionCutoff = 16;

// Sorts v[lo..hi] (inclusive) with insertion sort.
void insertion_sort_range(std::vector<int>& v, int lo, int hi) {
    for (int i = lo + 1; i <= hi; ++i) {
        int key = v[i];
        int j = i - 1;
        while (j >= lo && v[j] > key) {
            v[j + 1] = v[j];
            --j;
        }
        v[j + 1] = key;
    }
}

// Median of the three endpoints; used as the partition pivot value.
int median_of_three(const std::vector<int>& v, int lo, int hi) {
    int mid = lo + (hi - lo) / 2;
    const int a = v[lo];
    const int b = v[mid];
    const int c = v[hi];
    if (a < b) {
        if (b < c) {
            return b;  // a < b < c
        }
        if (a < c) {
            return c;  // a < c <= b
        }
        return a;  // c <= a < b
    }
    // b <= a
    if (a < c) {
        return a;  // b <= a < c
    }
    if (b < c) {
        return c;  // b < c <= a
    }
    return b;  // c <= b <= a
}

// Hoare partition around a pivot value: returns a split point j such that
// v[lo..j] <= pivot <= v[j+1..hi], with lo <= j < hi so both sides shrink.
int partition(std::vector<int>& v, int lo, int hi, int pivot) {
    int i = lo - 1;
    int j = hi + 1;
    for (;;) {
        do {
            ++i;
        } while (v[i] < pivot);
        do {
            --j;
        } while (v[j] > pivot);
        if (i >= j) {
            return j;
        }
        std::swap(v[i], v[j]);
    }
}

void quicksort_range(std::vector<int>& v, int lo, int hi) {
    while (lo < hi) {
        if (hi - lo + 1 <= kInsertionCutoff) {
            insertion_sort_range(v, lo, hi);
            return;
        }
        const int pivot = median_of_three(v, lo, hi);
        const int p = partition(v, lo, hi, pivot);
        // Recurse into the smaller side; loop on the larger one.
        if (p - lo < hi - p) {
            quicksort_range(v, lo, p);
            lo = p + 1;
        } else {
            quicksort_range(v, p + 1, hi);
            hi = p;
        }
    }
}

}  // namespace

// Sorts v in non-decreasing order, in place.
void quicksort(std::vector<int>& v) {
    if (v.size() > 1) {
        quicksort_range(v, 0, static_cast<int>(v.size()) - 1);
    }
}

static void check_against_reference(std::vector<int> v) {
    std::vector<int> expected = v;
    std::sort(expected.begin(), expected.end());
    quicksort(v);
    assert(v == expected);
}

static void run_tests() {
    // Empty input.
    std::vector<int> empty;
    quicksort(empty);
    assert(empty.empty());

    // Single element.
    std::vector<int> one{-7};
    quicksort(one);
    assert((one == std::vector<int>{-7}));

    // Two elements (partition boundary case).
    check_against_reference({2, 1});
    check_against_reference({1, 1});

    // Sorted, reverse sorted, duplicates, all equal.
    check_against_reference({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
                             15, 16, 17, 18, 19, 20});
    check_against_reference({20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9,
                             8, 7, 6, 5, 4, 3, 2, 1});
    check_against_reference({5, 1, 5, 3, 1, 9, 5, 3, 0, 9, 1});
    check_against_reference(std::vector<int>(40, 3));  // all equal, > cutoff

    // Negatives mixed with duplicates.
    check_against_reference({0, -1, 4, -1, -99, 4, 4, 12, -99, 7});

    // 200 fixed-seed shuffled arrays of varied sizes, including sizes that
    // straddle the insertion-sort cutoff.
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> size_dist(0, 400);
    std::uniform_int_distribution<int> val_dist(-1000, 1000);
    for (int t = 0; t < 200; ++t) {
        const int n = size_dist(rng);
        std::vector<int> v(n);
        for (int& x : v) {
            x = val_dist(rng);
        }
        std::shuffle(v.begin(), v.end(), rng);
        check_against_reference(v);
    }

    // A low-cardinality random case stresses duplicate handling further.
    std::uniform_int_distribution<int> tiny_dist(0, 3);
    for (int t = 0; t < 20; ++t) {
        std::vector<int> v(250);
        for (int& x : v) {
            x = tiny_dist(rng);
        }
        std::shuffle(v.begin(), v.end(), rng);
        check_against_reference(v);
    }
}

int main() {
    run_tests();
    std::cout << "All quicksort tests passed.\n\n";

    // Deterministic demo.
    std::vector<int> demo{9, -3, 4, 4, 0, 7, -3, 12, 1, 1, 5, 8, 2, 6, 11, 3,
                          10, 0, -7, 13};
    std::cout << "before:";
    for (int x : demo) {
        std::cout << ' ' << x;
    }
    std::cout << '\n';

    quicksort(demo);
    std::cout << "after: ";
    for (int x : demo) {
        std::cout << ' ' << x;
    }
    std::cout << '\n';
    return 0;
}
