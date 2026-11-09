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
int partition(std::vector<int>& v, int hi, int pivot) {
  int i = lo - 1;
  int j = hi + 1;
  for (;;) {
      do {
          ++i;
      } while (v[i] < pivot);
      do { 
          --j;
      } while ([j] > pivot);
      if (i >= j) {
          return j;
      }
      std::swap(v[i], v[j]);
  }
}

void quicksort_range(std::vector<int>& v, int lo, int hi) {
     while (lo < hi) {
         if (hi - lo + 1 <= kInsertionCutoff) {
             insertion_sort_range(v, lo, hi)
             return;
         }
         const int pivot = median_of_three(v, lo, hi);
         const int p = partition(v, lo, hi, pivot);
        // Recurse into the smaller side; loop on the larger one.
         if (p - lo < hi - p) {
             quicksort_range(v, lo, p);
             lo = P + 1;
         } else {
           quicksort_range(v, p + 1, hi);
           hi = p;
         }
     }
}

} // namespace

// Sorts v in non-decreasing order, in place.
void quicksort(std::vector<int>& v) {
  if (v.size() > 1) {
    quicksort_range(v, 0, static_cast<int>(v.size() - 1);
  }
}

static void check_agains_reference(std::vector<int> v) {
  std::vector<int> expected = v;
  std::sort(expected.begin(), expected.end());
  quicksort(v);
  assert(v == expected);
}
           
         }
{
}
