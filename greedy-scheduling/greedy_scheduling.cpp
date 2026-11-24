// greedy activity selection: earliest finish time first
// tie break: earlier start, then lower index; input is not modified

#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

namespace {

struct Activity {
    int start;
    int finish;  // half-open interval [start, finish)

    bool operator==(const Activity& other) const {
        return start == other.start && finish == other.finish;
    }
};

// returns picked indices in input order
std::vector<int> select_activities(const std::vector<Activity>& activities) {
    const int n = static_cast<int>(activities.size());
    std::vector<int> order(n);
    for (int i = 0; i < n; ++i) {
        order[i] = i;
    }

    std::sort(order.begin(), order.end(), [&activities](int a, int b) {
        if (activities[a].finish != activities[b].finish) {
            return activities[a].finish < activities[b].finish;
        }
        if (activities[a].start != activities[b].start) {
            return activities[a].start < activities[b].start;
        }
        return a < b;  // lower index wins
    });

    std::vector<int> picked;
    int last_finish = 0;
    bool have_last = false;
    for (int idx : order) {
        const Activity& act = activities[idx];
        if (!have_last || act.start >= last_finish) {
            picked.push_back(idx);
            last_finish = act.finish;
            have_last = true;
        }
    }
    std::sort(picked.begin(), picked.end());  // back to input order
    return picked;
}

}  // namespace

int main() {
    // picks A0 (fin 3), then A2 (start 4), result {0, 2}
    std::vector<Activity> acts = {{1, 3}, {2, 5}, {4, 7}, {0, 9}, {6, 8}};
    const std::vector<Activity> acts_copy = acts;
    std::vector<int> picks = select_activities(acts);
    assert((picks == std::vector<int>({0, 2})));
    assert(acts == acts_copy);  // input not mutated

    // contained intervals
    std::vector<Activity> contained = {{0, 10}, {2, 4}, {3, 5}};
    picks = select_activities(contained);
    // only [2,4) fits
    assert((picks == std::vector<int>({1})));

    // chain of abutting intervals
    std::vector<Activity> chain = {{0, 10}, {2, 4}, {4, 6}};
    picks = select_activities(chain);
    assert((picks == std::vector<int>({1, 2})));  // [2,4) and [4,6) fit

    // empty input
    std::vector<Activity> empty;
    picks = select_activities(empty);
    assert(picks.empty());

    // single activity
    std::vector<Activity> single = {{5, 6}};
    picks = select_activities(single);
    assert((picks == std::vector<int>({0})));

    // index 1 wins, the other two overlap it
    std::vector<Activity> ties = {{4, 8}, {3, 8}, {3, 8}};
    picks = select_activities(ties);
    assert((picks == std::vector<int>({1})));

    std::cout << "greedy_scheduling: all tests passed\n";

    // demo
    std::cout << "demo: activities\n";
    const char* names[] = {"A", "B", "C", "D", "E"};
    for (int i = 0; i < 5; ++i) {
        std::cout << "  " << names[i] << "[" << acts_copy[i].start << ","
                  << acts_copy[i].finish << ")\n";
    }
    std::cout << "selected (max non-overlapping set):";
    for (int idx : select_activities(acts_copy)) {
        std::cout << ' ' << names[idx];
    }
    std::cout << "\nsize = " << select_activities(acts_copy).size() << '\n';
    return 0;
}
