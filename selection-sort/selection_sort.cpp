// selection sort, in place + tests

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <vector>

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
    // empty input
    std::vector<int> empty;
    selection_sort(empty);
    assert(empty.empty());

    // single element
    std::vector<int> one{42};
    selection_sort(one);
    assert((one == std::vector<int>{42}));

    // already sorted
    std::vector<int> sorted{1, 2, 3, 4, 5};
    check_against_reference(sorted);

    // reverse sorted
    std::vector<int> reversed{9, 7, 5, 3, 1, 0, -2};
    check_against_reference(reversed);

    // duplicates
    std::vector<int> dup{5, 1, 5, 3, 1, 5, 0};
    check_against_reference(dup);

    // all equal
    std::vector<int> same(6, 7);
    selection_sort(same);
    assert((same == std::vector<int>(6, 7)));

    // negatives mixed in
    std::vector<int> negs{-3, 4, -1, 0, -3, 100, -50};
    check_against_reference(negs);

    // random array vs std::sort
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(-500, 500);
    std::vector<int> random(200);
    for (int& x : random) {
        x = dist(rng);
    }
    check_against_reference(random);

    // different size, same rng
    std::vector<int> random2(37);
    for (int& x : random2) {
        x = dist(rng);
    }
    check_against_reference(random2);

    // check the output is sorted
    selection_sort(random2);
    assert(is_sorted_non_decreasing(random2));
}

int main() {
    run_tests();
    std::cout << "All selection sort tests passed.\n\n";

    // demo run
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
