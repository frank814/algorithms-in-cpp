// Binary search over a sorted std::vector<int>.
//
// binary_search_iter / binary_search_rec: locate a value in a sorted vector,
//   returning its index or -1 when absent.
// lower_bound_idx: first position whose value is >= target (also the insertion
//   point for target; equals the index of the first occurrence if present).
// upper_bound_idx: first position whose value is > target (index just past the
//   last occurrence of target).
//
// All variants run in O(log n) time and O(1) extra space (recursion uses
// O(log n) stack frames).

#include <cassert>
#include <iostream>
#include <vector>

// Iterative binary search: returns an index of target, or -1 if not found.
int binary_search_iter(const std::vector<int>& v, int target) {
    int lo = 0;
    int hi = static_cast<int>(v.size()) - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;  // avoids overflow vs. (lo + hi) / 2
        if (v[mid] == target) {
            return mid;
        }
        if (v[mid] < target) {
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }
    return -1;
}

// Recursive binary search with the same contract as above.
int binary_search_rec(const std::vector<int>& v, int target, int lo, int hi) {
    if (lo > hi) {
        return -1;  // empty search range
    }
    int mid = lo + (hi - lo) / 2;
    if (v[mid] == target) {
        return mid;
    }
    if (v[mid] < target) {
        return binary_search_rec(v, target, mid + 1, hi);
    }
    return binary_search_rec(v, target, lo, mid - 1);
}

// Convenience wrapper starting over the whole vector.
int binary_search_rec(const std::vector<int>& v, int target) {
    return binary_search_rec(v, target, 0, static_cast<int>(v.size()) - 1);
}

// First index i with v[i] >= target; returns v.size() if all elements are
// smaller. On an empty vector this is 0.
int lower_bound_idx(const std::vector<int>& v, int target) {
    int lo = 0;
    int hi = static_cast<int>(v.size());  // exclusive
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (v[mid] < target) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    return lo;
}

// First index i with v[i] > target; returns v.size() if no element is larger.
int upper_bound_idx(const std::vector<int>& v, int target) {
    int lo = 0;
    int hi = static_cast<int>(v.size());  // exclusive
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (v[mid] <= target) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    return lo;
}

static void run_tests() {
    // Empty vector.
    const std::vector<int> empty;
    assert(binary_search_iter(empty, 5) == -1);
    assert(binary_search_rec(empty, 5) == -1);
    assert(lower_bound_idx(empty, 5) == 0);
    assert(upper_bound_idx(empty, 5) == 0);

    // Single element.
    const std::vector<int> one{7};
    assert(binary_search_iter(one, 7) == 0);
    assert(binary_search_rec(one, 7) == 0);
    assert(binary_search_iter(one, 3) == -1);
    assert(binary_search_iter(one, 9) == -1);
    assert(lower_bound_idx(one, 7) == 0);
    assert(upper_bound_idx(one, 7) == 1);
    assert(lower_bound_idx(one, 8) == 1);
    assert(upper_bound_idx(one, 6) == 0);

    const std::vector<int> v{1, 3, 5, 5, 5, 9, 12};

    // Present value: both implementations return a valid index.
    int idx = binary_search_iter(v, 5);
    assert(idx >= 2 && idx <= 4 && v[idx] == 5);
    idx = binary_search_rec(v, 5);
    assert(idx >= 2 && idx <= 4 && v[idx] == 5);
    assert(binary_search_iter(v, 1) == 0);
    assert(binary_search_iter(v, 12) == 6);
    assert(binary_search_rec(v, 12) == 6);

    // Below / above the whole range.
    assert(binary_search_iter(v, 0) == -1);
    assert(binary_search_rec(v, 0) == -1);
    assert(binary_search_iter(v, 100) == -1);
    assert(binary_search_rec(v, 100) == -1);

    // Absent interior value.
    assert(binary_search_iter(v, 4) == -1);
    assert(binary_search_rec(v, 4) == -1);
    assert(binary_search_iter(v, 7) == -1);
    assert(binary_search_rec(v, 7) == -1);

    // Duplicates: first / last occurrence boundaries.
    assert(lower_bound_idx(v, 5) == 2);
    assert(upper_bound_idx(v, 5) == 5);
    assert(lower_bound_idx(v, 1) == 0);
    assert(upper_bound_idx(v, 12) == 7);
    // Absent value: both bounds coincide at the insertion point.
    assert(lower_bound_idx(v, 4) == 2);
    assert(upper_bound_idx(v, 4) == 2);
    assert(lower_bound_idx(v, 100) == 7);
    assert(upper_bound_idx(v, 0) == 0);

    // All equal.
    const std::vector<int> same(5, 2);
    assert(lower_bound_idx(same, 2) == 0);
    assert(upper_bound_idx(same, 2) == 5);
    int eq = binary_search_iter(same, 2);
    assert(eq >= 0 && eq < 5);  // any occurrence is a valid hit
    eq = binary_search_rec(same, 2);
    assert(eq >= 0 && eq < 5);
    assert(binary_search_iter(same, 9) == -1);
}

int main() {
    run_tests();
    std::cout << "All binary search tests passed.\n\n";

    // Deterministic demo.
    const std::vector<int> data{1, 3, 5, 5, 5, 9, 12};
    std::cout << "Sorted data:";
    for (int x : data) {
        std::cout << ' ' << x;
    }
    std::cout << '\n';

    for (int target : {5, 12, 4, 100}) {
        std::cout << "target=" << target
                  << "  iter=" << binary_search_iter(data, target)
                  << "  rec=" << binary_search_rec(data, target)
                  << "  lower=" << lower_bound_idx(data, target)
                  << "  upper=" << upper_bound_idx(data, target) << '\n';
    }

    std::cout << "empty lower_bound(5)=" << lower_bound_idx({}, 5) << '\n';
    return 0;
}
