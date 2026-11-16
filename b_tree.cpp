// b_tree.cpp
// B-tree of minimum degree t = 3 (nodes hold between t-1 = 2 and 2t-1 = 5
// keys, the root may hold 1..5). Supports insert with node splitting,
// search and in-order traversal (which yields all keys sorted).
// Duplicate policy: duplicates are ignored.
//
// Time complexity: O(t log_t n) = O(log n) per insert/search,
// O(n) in-order traversal. Space: O(n).

#include <algorithm>
#include <cassert>
#include <climits>
#include <iostream>
#include <vector>

class BTree {
public:
    static constexpr int kMinDegree = 3;
    static constexpr int kMaxKeys = 2 * kMinDegree - 1;  // 5
    static constexpr int kMinKeys = kMinDegree - 1;      // 2

    struct Node {
        bool leaf;
        std::vector<int> keys;
        std::vector<Node*> children;  // empty for leaves, keys.size()+1 else
        explicit Node(bool is_leaf) : leaf(is_leaf) {}
    };

    BTree() = default;
    ~BTree() { clear(root_); }

    BTree(const BTree&) = delete;
    BTree& operator=(const BTree&) = delete;

    bool insert(int key) {
        if (root_ != nullptr && searchNode(root_, key) != nullptr) {
            return false;  // duplicates ignored
        }
        if (root_ == nullptr) {
            root_ = new Node(true);
            root_->keys.push_back(key);
            return true;
        }
        Node* r = root_;
        // If the root is full it splits before descending.
        if (static_cast<int>(r->keys.size()) == kMaxKeys) {
            Node* newRoot = new Node(false);
            newRoot->children.push_back(r);
            splitChild(newRoot, 0);
            root_ = newRoot;
        }
        insertNonFull(root_, key);
        return true;
    }

    bool search(int key) const {
        return root_ != nullptr && searchNode(root_, key) != nullptr;
    }

    // All keys in ascending order.
    std::vector<int> in_order() const {
        std::vector<int> out;
        collect(root_, out);
        return out;
    }

    bool empty() const { return root_ == nullptr; }
    Node* root() const { return root_; }

    // Structural verification:
    //   - keys strictly increasing inside (lo, hi),
    //   - every non-root node holds [t-1, 2t-1] keys (root: [0, 2t-1]),
    //   - internal node has keys.size()+1 children,
    //   - all leaves sit at the same depth (returned).
    int verify(const Node* x, int depth, int& leaf_depth,
               long lo, long hi) const {
        const int n = static_cast<int>(x->keys.size());
        if (x == root_) {
            assert(n >= 0 && n <= kMaxKeys);
            if (!x->leaf) assert(n >= 1);
        } else {
            assert(n >= kMinKeys && n <= kMaxKeys);
        }
        for (int i = 0; i < n; ++i) {
            assert(lo < x->keys[i] && x->keys[i] < hi);
            if (i > 0) assert(x->keys[i - 1] < x->keys[i]);
        }
        if (x->leaf) {
            assert(x->children.empty());
            if (leaf_depth < 0) leaf_depth = depth;
            assert(leaf_depth == depth);  // uniform leaf depth
            return depth;
        }
        assert(static_cast<int>(x->children.size()) == n + 1);
        for (int i = 0; i <= n; ++i) {
            long clo = (i == 0) ? lo : x->keys[i - 1];
            long chi = (i == n) ? hi : x->keys[i];
            verify(x->children[i], depth + 1, leaf_depth, clo, chi);
        }
        return depth;
    }

    // Convenience wrapper: checks the whole tree, returns leaf depth.
    int verifyAll() const {
        if (root_ == nullptr) return 0;
        int leaf_depth = -1;
        verify(root_, 0, leaf_depth, LONG_MIN, LONG_MAX);
        return leaf_depth;
    }

private:
    Node* root_ = nullptr;

    static Node* searchNode(Node* x, int key) {
        int i = 0;
        while (i < static_cast<int>(x->keys.size()) && key > x->keys[i]) ++i;
        if (i < static_cast<int>(x->keys.size()) && key == x->keys[i]) {
            return x;
        }
        if (x->leaf) return nullptr;
        return searchNode(x->children[i], key);
    }

    // Splits the full child x->children[i] (2t-1 keys) into two nodes and
    // pushes the median key up into x.
    void splitChild(Node* x, int i) {
        Node* y = x->children[i];
        Node* z = new Node(y->leaf);  // receives the upper half

        // y keeps keys [0, t-2]; the median (index t-1) rises to x;
        // z takes keys [t, 2t-2].
        int medianKey = y->keys[kMinKeys];
        z->keys.assign(y->keys.begin() + kMinDegree, y->keys.end());
        y->keys.resize(kMinKeys);
        if (!y->leaf) {
            // Children are split as t / t around the median.
            z->children.assign(y->children.begin() + kMinDegree,
                               y->children.end());
            y->children.resize(kMinDegree);
        }

        x->keys.insert(x->keys.begin() + i, medianKey);
        x->children.insert(x->children.begin() + i + 1, z);
    }

    // Inserts into a node that is guaranteed not to be full.
    void insertNonFull(Node* x, int key) {
        if (x->leaf) {
            auto pos = std::upper_bound(x->keys.begin(), x->keys.end(), key);
            x->keys.insert(pos, key);
            return;
        }
        int i = static_cast<int>(x->keys.size()) - 1;
        while (i >= 0 && key < x->keys[i]) --i;
        ++i;  // child index to descend into

        Node* child = x->children[i];
        if (static_cast<int>(child->keys.size()) == kMaxKeys) {
            splitChild(x, i);
            // The median moved up; decide which side holds the key.
            if (key > x->keys[i]) ++i;
            else if (key == x->keys[i]) return;  // paranoia: duplicates
            child = x->children[i];
        }
        insertNonFull(child, key);
    }

    static void collect(const Node* x, std::vector<int>& out) {
        if (x == nullptr) return;
        if (x->leaf) {
            out.insert(out.end(), x->keys.begin(), x->keys.end());
            return;
        }
        for (size_t i = 0; i < x->children.size(); ++i) {
            collect(x->children[i], out);
            if (i < x->keys.size()) out.push_back(x->keys[i]);
        }
    }

    static void clear(Node* x) {
        if (x == nullptr) return;
        for (Node* c : x->children) clear(c);
        delete x;
    }
};

// ---------------------------------------------------------------------------
int main() {
    // --- 100 sequential keys -------------------------------------------------
    {
        BTree t;
        std::vector<int> expected;
        for (int k = 1; k <= 100; ++k) {
            assert(t.insert(k));
            expected.push_back(k);
            if (k % 10 == 0) {
                t.verifyAll();
                assert(t.in_order() == expected);
            }
        }
        assert(t.verifyAll() >= 1);  // 100 keys no longer fit in one node
        assert(t.in_order() == expected);
        std::cout << "100 sequential keys: in-order sorted, leaf depth "
                  << t.verifyAll() << '\n';
    }

    // --- plus 100 shuffled keys ---------------------------------------------
    {
        BTree t;
        std::vector<int> all;
        for (int k = 1; k <= 100; ++k) {
            assert(t.insert(k));
            all.push_back(k);
        }
        // Deterministic shuffle of 101..200 (LCG-based Fisher-Yates).
        std::vector<int> shuffled;
        for (int k = 101; k <= 200; ++k) shuffled.push_back(k);
        unsigned seed = 42u;
        for (size_t i = shuffled.size(); i-- > 1; ) {
            seed = seed * 1664525u + 1013904223u;
            size_t j = seed % (i + 1);
            std::swap(shuffled[i], shuffled[j]);
        }
        for (int k : shuffled) {
            assert(t.insert(k));
            all.push_back(k);
        }
        std::sort(all.begin(), all.end());

        t.verifyAll();                       // key counts + uniform leaf depth
        assert(t.in_order() == all);         // everything sorted, nothing lost
        assert(t.in_order().size() == 200);
        std::cout << "100 shuffled more keys: 200 total, in-order sorted\n";
    }

    // --- Node key counts and uniform depth (explicit assertions) ------------
    {
        BTree t;
        for (int k = 1; k <= 300; ++k) assert(t.insert(k));
        int depth = t.verifyAll();
        // verifyAll already asserts every node count is within
        // [t-1, 2t-1] (root exempt at the low end) and all leaves share
        // the same depth; restate the guarantees here:
        assert(depth >= 1);
        assert(t.in_order().size() == 300);
        // With t = 3 and 300 keys the tree cannot be deeper than ~7.
        assert(depth <= 7);
        std::cout << "300 keys: all nodes within [2,5], leaves at depth "
                  << depth << '\n';
    }

    // --- Search hits and misses ---------------------------------------------
    {
        BTree t;
        for (int k = 1; k <= 50; ++k) assert(t.insert(k));
        for (int k : {1, 25, 50, 7, 33}) assert(t.search(k));
        for (int k : {0, 51, -10, 999}) assert(!t.search(k));
        std::cout << "search: hits and misses OK\n";
    }

    // --- Duplicates ignored --------------------------------------------------
    {
        BTree t;
        assert(t.insert(10));
        assert(!t.insert(10));
        assert(t.insert(5));
        assert(!t.insert(5));
        assert((t.in_order() == std::vector<int>{5, 10}));
        std::cout << "duplicates: ignored\n";
    }

    // --- Small / empty trees -------------------------------------------------
    {
        BTree t;
        assert(t.empty());
        assert(!t.search(1));
        assert(t.in_order().empty());
        t.verifyAll();

        assert(t.insert(42));
        t.verifyAll();
        assert((t.in_order() == std::vector<int>{42}));
        assert(t.search(42));
        std::cout << "empty/single-key tree: OK\n";
    }

    // --- Key order stress: descending and alternating patterns --------------
    {
        BTree t;
        std::vector<int> expected;
        for (int k = 500; k >= 1; --k) {  // descending (splits everywhere)
            assert(t.insert(k));
            expected.push_back(k);
        }
        std::sort(expected.begin(), expected.end());
        t.verifyAll();
        assert(t.in_order() == expected);
        std::cout << "500 descending keys: in-order sorted\n";
    }

    std::cout << "b_tree: all tests passed\n";
    return 0;
}
