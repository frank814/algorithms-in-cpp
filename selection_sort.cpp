// In-place selection sort.
//
// Repeatedly scans the unsorted suffix for its minimum and swaps it into
// place at the front of the sorted prefix.
//
// Time:  O(n^2) comparisons in all cases (best / average / worst).
// Space: O(1) extra — the sort happens in place.

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <vector>

// Sorts v in non-decreasing order using selection sort.
void selection_sort(std::vector<int>& v) {
    const int n = static_cast<int>(v.size());
    for (int i = 0; i < n; ++i) {
        int min_pos = i;
        for (int j = i + 1; j < n; ++j) {
            if (v[j] < v[min_pos]) {
                min_pos = j;
            }
        }
        if (min_pos != i) {
            std::swap(v[i], v[min_pos]);
        }
    }
}

// Returns true when v is sorted in non-decreasing order.
bool is_sorted_non_decreasing(const std::vector<int>& v) {
    for (std::size_t i = 1; i < v.size(); ++i) {
        if (v[i - 1] > v[i]) {
            return false;
        }
    }
    return true;
}

static void check_against_reference(std::vector<int> v) {
    std::vector<int> expected = v;
    std::sort(expected.begin(), expected.end());
    selection_sort(v);
    assert(v == expected);
}

static void run_tests() {
    // Empty input.
    std::vector<int> empty;
    selection_sort(empty);
    assert(empty.empty());

    // Single element.
    std::vector<int> one{42};
    selection_sort(one);
    assert((one == std::vector<int>{42}));

    // Already sorted (stability does not matter for correctness here).
    std::vector<int> sorted{1, 2, 3, 4, 5};
    check_against_reference(sorted);

    // Reverse sorted.
    std::vector<int> reversed{9, 7, 5, 3, 1, 0, -2};
    check_against_reference(reversed);

    // Duplicates.
    std::vector<int> dup{5, 1, 5, 3, 1, 5, 0};
    check_against_reference(dup);

    // All equal.
    std::vector<int> same(6, 7);
    selection_sort(same);
    assert((same == std::vector<int>(6, 7)));

    // Negatives mixed in.
    std::vector<int> negs{-3, 4, -1, 0, -3, 100, -50};
    check_against_reference(negs);

    // Fixed-seed random array vs. std::sort reference.
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(-500, 500);
    std::vector<int> random(200);
    for (int& x : random) {
        x = dist(rng);
    }
    check_against_reference(random);

    // A second, differently sized random case reuses the same seed stream.
    std::vector<int> random2(37);
    for (int& x : random2) {
        x = dist(rng);
    }
    check_against_reference(random2);

    // Sortedness property on a random array.
    selection_sort(random2);
    assert(is_sorted_non_decreasing(random2));
}

int main() {
    run_tests();
    std::cout << "All selection sort tests passed.\n\n";

    // Deterministic demo.
    std::vector<int> demo{-5, 3, 0, 3, -11, 8};
    std::cout << "before:";
    for (int x : demo) {
        std::cout << ' ' << x;
    }
    std::cout << '\n';

    selection_sort(demo);
    std::cout << "after: ";
    for (int x : demo) {
        std::cout << ' ' << x;
    }
    std::cout << '\n';
    return 0;
}
