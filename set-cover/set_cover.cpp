// greedy set cover: pick the set covering the most new elements each round
// stops early if no set adds anything, so partial coverage is possible

#include <cassert>
#include <iostream>
#include <vector>

namespace {

struct CoverResult {
    std::vector<int> chosen;  // picked set indices, in pick order
    int covered;              // elements covered
};

// elements are labelled 0..universe_size-1; duplicate labels count once
CoverResult greedy_set_cover(int universe_size,
                             const std::vector<std::vector<int>>& sets) {
    std::vector<char> is_covered(universe_size, 0);
    int covered = 0;

    std::vector<char> chosen_flag(sets.size(), 0);
    CoverResult result;
    result.covered = 0;

    while (covered < universe_size) {
        // most new elements wins
        int best = -1;
        int best_gain = 0;
        for (int s = 0; s < static_cast<int>(sets.size()); ++s) {
            if (chosen_flag[s]) {
                continue;
            }
            int gain = 0;
            for (int elem : sets[s]) {
                if (elem >= 0 && elem < universe_size &&
                    !is_covered[elem]) {
                    ++gain;
                }
            }
            if (gain > best_gain) {
                best_gain = gain;
                best = s;
            }
        }

        if (best == -1) {
            break;  // nothing new to cover
        }

        chosen_flag[best] = 1;
        result.chosen.push_back(best);
        for (int elem : sets[best]) {
            if (elem >= 0 && elem < universe_size && !is_covered[elem]) {
                is_covered[elem] = 1;
                ++covered;
            }
        }
    }

    result.covered = covered;
    return result;
}

}  // namespace

int main() {
    // round 1: S2 gains 4 and wins; round 2: S0 adds 0,1. result {2, 0}
    {
        std::vector<std::vector<int>> sets = {{0, 1, 2}, {3, 4}, {2, 3, 4, 5}};
        CoverResult r = greedy_set_cover(6, sets);
        assert((r.chosen == std::vector<int>({2, 0})));
        assert(r.covered == 6);
    }

    // overlapping sets, each element only counts once
    // round 1 tie goes to S0, round 2 S2 gains more. result {0, 2}
    {
        std::vector<std::vector<int>> sets = {{0, 1}, {1, 2}, {2, 3}};
        CoverResult r = greedy_set_cover(4, sets);
        assert((r.chosen == std::vector<int>({0, 2})));
        assert(r.covered == 4);
    }

    // empty universe
    {
        std::vector<std::vector<int>> sets = {{0}, {1}};
        CoverResult r = greedy_set_cover(0, sets);
        assert(r.chosen.empty());
        assert(r.covered == 0);
    }

    // elements 3,4 can never be covered, stops at covered = 3
    {
        std::vector<std::vector<int>> sets = {{0, 1}, {2}};
        CoverResult r = greedy_set_cover(5, sets);
        assert((r.chosen == std::vector<int>({0, 1})));
        assert(r.covered == 3);  // partial coverage
    }

    // no sets
    {
        std::vector<std::vector<int>> sets;
        CoverResult r = greedy_set_cover(3, sets);
        assert(r.chosen.empty());
        assert(r.covered == 0);
    }

    std::cout << "set_cover: all tests passed\n";

    // demo
    std::vector<std::vector<int>> demo = {{0, 1, 2}, {3, 4}, {2, 3, 4, 5}};
    CoverResult rd = greedy_set_cover(6, demo);
    std::cout << "demo: universe {0..5}, sets: S0={0,1,2} S1={3,4} "
                 "S2={2,3,4,5}\n";
    std::cout << "greedy picks:";
    for (int s : rd.chosen) {
        std::cout << " S" << s;
    }
    std::cout << "\ncovered elements = " << rd.covered << " / 6\n";
    return 0;
}
