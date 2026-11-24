// fractional knapsack: greedily take items by value/weight ratio
// this greedy does not work for 0/1 knapsack, see dp_knapsack

#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

namespace {

struct Item {
    long long value;
    long long weight;
};

struct FractionalResult {
    double max_value;
    std::vector<double> fraction;  // part of item i taken, in [0,1]
};

FractionalResult fractional_knapsack(const std::vector<Item>& items,
                                     long long capacity) {
    if (capacity < 0) {
        capacity = 0;
    }

    const int n = static_cast<int>(items.size());
    std::vector<int> order(n);
    for (int i = 0; i < n; ++i) {
        order[i] = i;
    }

    auto ratio = [&items](int i) {
        return items[i].weight > 0
                   ? static_cast<double>(items[i].value) / items[i].weight
                   : (items[i].value > 0 ? 1e18 : 0.0);
    };

    // highest ratio first, lower index wins ties
    std::sort(order.begin(), order.end(), [&ratio](int a, int b) {
        const double ra = ratio(a), rb = ratio(b);
        if (ra != rb) {
            return ra > rb;
        }
        return a < b;
    });

    FractionalResult res;
    res.max_value = 0.0;
    res.fraction.assign(n, 0.0);

    long long remaining = capacity;
    for (int idx : order) {
        if (remaining <= 0) {
            break;
        }
        const Item& it = items[idx];
        if (it.weight <= remaining) {
            res.fraction[idx] = 1.0;
            res.max_value += static_cast<double>(it.value);
            remaining -= it.weight;
        } else if (it.weight > 0) {
            const double f = static_cast<double>(remaining) / it.weight;
            res.fraction[idx] = f;
            res.max_value += f * static_cast<double>(it.value);
            remaining = 0;
        }
    }
    return res;
}

// sum of fraction * value, used by tests
double total_value(const std::vector<Item>& items,
                   const std::vector<double>& fraction) {
    double sum = 0.0;
    for (std::size_t i = 0; i < items.size(); ++i) {
        sum += fraction[i] * static_cast<double>(items[i].value);
    }
    return sum;
}

bool near(double a, double b, double eps = 1e-9) {
    return (a > b ? a - b : b - a) <= eps;
}

}  // namespace

int main() {
    // exact fit
    {
        std::vector<Item> items = {{60, 10}, {100, 20}, {120, 30}};
        FractionalResult r = fractional_knapsack(items, 50);
        // ratios 6,5,4: all of items 0 and 1, then 20/30 of item 2 = 240
        assert(near(r.max_value, 240.0));
        assert(near(r.fraction[0], 1.0));
        assert(near(r.fraction[1], 1.0));
        assert(near(r.fraction[2], 20.0 / 30.0));
        assert(near(r.max_value, total_value(items, r.fraction)));
    }

    // capacity 0
    {
        std::vector<Item> items = {{5, 1}};
        FractionalResult r = fractional_knapsack(items, 0);
        assert(near(r.max_value, 0.0));
        assert(near(r.fraction[0], 0.0));
    }

    // item too heavy, take a fraction
    {
        std::vector<Item> items = {{100, 10}};
        FractionalResult r = fractional_knapsack(items, 4);
        assert(near(r.max_value, 40.0));
        assert(near(r.fraction[0], 0.4));
    }

    // equal ratios
    {
        std::vector<Item> items = {{4, 2}, {6, 3}, {8, 4}};
        FractionalResult r = fractional_knapsack(items, 5);
        // ratio 2 everywhere, 5 units * 2 = 10
        assert(near(r.max_value, 10.0));
        assert(near(total_value(items, r.fraction), 10.0));
        double used_weight = 0.0;
        for (std::size_t i = 0; i < items.size(); ++i) {
            used_weight += r.fraction[i] * static_cast<double>(items[i].weight);
        }
        assert(near(used_weight, 5.0));
    }

    // cross-check against the best of all item orderings
    {
        std::vector<Item> items = {{20, 5}, {15, 4}, {30, 9}, {10, 3}};
        const long long cap = 12;
        FractionalResult r = fractional_knapsack(items, cap);

        // best over all 24 orderings
        std::vector<int> perm = {0, 1, 2, 3};
        double best = 0.0;
        do {
            long long rem = cap;
            double val = 0.0;
            for (int i : perm) {
                if (rem <= 0) {
                    break;
                }
                const long long take = std::min(rem, items[i].weight);
                val += static_cast<double>(take) *
                       static_cast<double>(items[i].value) /
                       static_cast<double>(items[i].weight);
                rem -= take;
            }
            best = std::max(best, val);
        } while (std::next_permutation(perm.begin(), perm.end()));

        assert(near(r.max_value, best));
        assert(near(r.max_value, total_value(items, r.fraction)));
    }

    std::cout << "greedy_knapsack: all tests passed\n";

    // demo
    std::vector<Item> demo = {{60, 10}, {100, 20}, {120, 30}};
    FractionalResult rd = fractional_knapsack(demo, 50);
    std::cout << "demo: capacity 50, items (value, weight) = (60,10) (100,20) "
                 "(120,30)\n";
    for (std::size_t i = 0; i < demo.size(); ++i) {
        std::cout << "  item " << i << " taken fraction = " << rd.fraction[i]
                  << '\n';
    }
    std::cout << "max value = " << rd.max_value << '\n';
    return 0;
}
