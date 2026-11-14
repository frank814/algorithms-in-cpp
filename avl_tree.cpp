// avl_tree.cpp
// Self-balancing AVL binary search tree: insert and delete with single/double
// rotations (zig-zig, zig-zag cases). Every node stores its height; the
// balance factor of every node stays in {-1, 0, 1}.
// Duplicate policy: duplicates are ignored.
//
// Time complexity: O(log n) for insert/search/delete, O(n) traversal.
// Space: O(n); recursion depth O(log n).
// Height bound: h <= 1.44 * log2(n + 2) (height counted in edges, leaf = 0).

#include <algorithm>
#include <cassert>
#include <climits>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

class AVL {
public:
    struct Node {
        int key;
        int height;  // edges on the longest path down to a leaf; leaf = 0
        Node* left;
        Node* right;
        Node(int k) : key(k), height(0), left(nullptr), right(nullptr) {}
    };

    AVL() = default;
    ~AVL() { clear(root_); }

    AVL(const AVL&) = delete;
    AVL& operator=(const AVL&) = delete;

    bool insert(int key) { return insertRec(root_, key); }

    bool remove(int key) { return removeRec(root_, key); }

    bool search(int key) const {
        for (Node* n = root_; n != nullptr;) {
            if (key == n->key) return true;
            n = (key < n->key) ? n->left : n->right;
        }
        return false;
    }

    std::vector<int> in_order() const {
        std::vector<int> out;
        walk(root_, out);
        return out;
    }

    int height() const { return root_ == nullptr ? -1 : root_->height; }
    bool empty() const { return root_ == nullptr; }
    Node* root() const { return root_; }

    static int heightOf(const Node* n) { return n == nullptr ? -1 : n->height; }

    // Verifies the AVL property recursively:
    //   - BST order within [lo, hi) bounds,
    //   - balance factor in {-1, 0, 1},
    //   - stored height matches recomputed height.
    // Returns the recomputed height (edges).
    static int verify(const Node* n, long lo, long hi) {
        if (n == nullptr) return -1;
        assert(lo < n->key && n->key < hi);
        int hl = verify(n->left, lo, n->key);
        int hr = verify(n->right, n->key, hi);
        assert(std::abs(hl - hr) <= 1);
        assert(n->height == 1 + std::max(hl, hr));
        return n->height;
    }

private:
    Node* root_ = nullptr;

    static int update(Node* n) {
        n->height = 1 + std::max(heightOf(n->left), heightOf(n->right));
        return n->height;
    }

    static int balanceOf(const Node* n) {
        return heightOf(n->left) - heightOf(n->right);
    }

    // Right rotation (zig):       y            x
    //                           x   C   ->   A   y
    //                         A  B            B  C
    static Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* B = x->right;
        x->right = y;
        y->left = B;
        update(y);
        update(x);
        return x;
    }

    // Left rotation (zag): mirror of rotateRight.
    static Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* B = y->left;
        y->left = x;
        x->right = B;
        update(x);
        update(y);
        return y;
    }

    // Restores the AVL property at `n` after a child changed.
    static Node* rebalance(Node* n) {
        update(n);
        int b = balanceOf(n);
        if (b > 1) {                      // left-heavy
            if (balanceOf(n->left) < 0) { // zig-zag: left-right case
                n->left = rotateLeft(n->left);
            }
            return rotateRight(n);        // zig-zig: left-left case
        }
        if (b < -1) {                     // right-heavy
            if (balanceOf(n->right) > 0) { // zig-zag: right-left case
                n->right = rotateRight(n->right);
            }
            return rotateLeft(n);         // zig-zig: right-right case
        }
        return n;
    }

    static bool insertRec(Node*& n, int key) {
        if (n == nullptr) {
            n = new Node(key);
            return true;
        }
        if (key == n->key) return false;  // duplicates ignored
        bool added = (key < n->key) ? insertRec(n->left, key)
                                    : insertRec(n->right, key);
        if (added) n = rebalance(n);
        return added;
    }

    // Removes the in-order successor of `n` from its right subtree and
    // returns its key; used when both children are present.
    static int takeSuccessor(Node*& n) {
        if (n->left == nullptr) {
            Node* doomed = n;
            int k = n->key;
            n = n->right;
            delete doomed;
            return k;
        }
        int k = takeSuccessor(n->left);
        n = rebalance(n);
        return k;
    }

    static bool removeRec(Node*& n, int key) {
        if (n == nullptr) return false;
        bool removed;
        if (key < n->key) {
            removed = removeRec(n->left, key);
        } else if (key > n->key) {
            removed = removeRec(n->right, key);
        } else {
            removed = true;
            if (n->left == nullptr || n->right == nullptr) {
                Node* doomed = n;
                n = (n->left != nullptr) ? n->left : n->right;
                delete doomed;
            } else {
                // Two children: overwrite with in-order successor's key.
                n->key = takeSuccessor(n->right);
            }
        }
        if (removed && n != nullptr) n = rebalance(n);
        return removed;
    }

    static void walk(const Node* n, std::vector<int>& out) {
        if (n == nullptr) return;
        walk(n->left, out);
        out.push_back(n->key);
        walk(n->right, out);
    }

    static void clear(Node* n) {
        if (n == nullptr) return;
        clear(n->left);
        clear(n->right);
        delete n;
    }
};

// ---------------------------------------------------------------------------
int main() {
    // Helper: verify AVL invariants + sorted in-order after every operation.
    auto check = [](AVL& t, const std::vector<int>& expected) {
        AVL::verify(t.root(), LONG_MIN, LONG_MAX);
        assert(t.in_order() == expected);
    };

    // --- Insert a sequence -> in-order sorted, AVL property holds -----------
    {
        AVL t;
        std::vector<int> keys = {50, 30, 70, 20, 40, 60, 80, 45, 65, 35};
        std::vector<int> sorted;
        for (int k : keys) {
            assert(t.insert(k));
            sorted.push_back(k);
            std::sort(sorted.begin(), sorted.end());
            check(t, sorted);
        }
        assert(!t.insert(45));  // duplicates ignored
        std::cout << "inserts: AVL invariants hold after every op\n";
    }

    // --- All four rotation cases --------------------------------------------
    {
        // Left-left (right rotation): 30 20 10
        AVL ll;
        for (int k : {30, 20, 10}) assert(ll.insert(k));
        check(ll, {10, 20, 30});
        assert(ll.root()->key == 20);

        // Right-right (left rotation): 10 20 30
        AVL rr;
        for (int k : {10, 20, 30}) assert(rr.insert(k));
        check(rr, {10, 20, 30});
        assert(rr.root()->key == 20);

        // Left-right (double): 30 10 20
        AVL lr;
        for (int k : {30, 10, 20}) assert(lr.insert(k));
        check(lr, {10, 20, 30});
        assert(lr.root()->key == 20);

        // Right-left (double): 10 30 20
        AVL rl;
        for (int k : {10, 30, 20}) assert(rl.insert(k));
        check(rl, {10, 20, 30});
        assert(rl.root()->key == 20);

        std::cout << "rotation cases: LL/RR/LR/RL all balanced\n";
    }

    // --- Sorted insert worst case stays shallow -----------------------------
    {
        AVL t;
        const int n = 1000;
        for (int k = 0; k < n; ++k) assert(t.insert(k));
        int h = t.height();
        double bound = 1.44 * std::log2(static_cast<double>(n + 2));
        std::cout << "sorted insert n=" << n << " height=" << h
                  << " bound=" << bound << '\n';
        assert(static_cast<double>(h) <= bound);
        // Compare against an unbalanced BST of the same insertions (height n-1).
        assert(h < n - 1);

        std::vector<int> expected(n);
        for (int k = 0; k < n; ++k) expected[k] = k;
        check(t, expected);
    }

    // --- Randomised workload keeps the invariants ---------------------------
    {
        AVL t;
        std::vector<int> present;
        unsigned seed = 12345u;
        auto nextRand = [&seed]() {
            seed = seed * 1664525u + 1013904223u;  // LCG, deterministic
            return seed % 200;
        };
        for (int i = 0; i < 500; ++i) {
            int k = static_cast<int>(nextRand());
            bool did = t.insert(k);
            bool was = std::find(present.begin(), present.end(), k) != present.end();
            assert(did == !was);
            if (did) present.push_back(k);
            std::sort(present.begin(), present.end());
            check(t, present);
        }
        for (int i = 0; i < 250; ++i) {
            int k = static_cast<int>(nextRand());
            bool did = t.remove(k);
            auto it = std::find(present.begin(), present.end(), k);
            assert(did == (it != present.end()));
            if (did) present.erase(it);
            check(t, present);
        }
        std::cout << "randomised workload: invariants hold (" << present.size()
                  << " keys left)\n";
    }

    // --- Deletes of root / middle / leaf ------------------------------------
    {
        AVL t;
        std::vector<int> keys = {40, 20, 60, 10, 30, 50, 70, 5, 15, 25, 35};
        std::vector<int> present = keys;
        std::sort(present.begin(), present.end());
        for (int k : keys) assert(t.insert(k));
        check(t, present);

        // Leaf delete (5), middle delete (20), root delete (40).
        for (int k : {5, 20, 40}) {
            assert(t.remove(k));
            present.erase(std::find(present.begin(), present.end(), k));
            check(t, present);
        }
        assert(!t.search(40));
        assert(!t.remove(40));   // already gone
        assert(!t.remove(999));

        // Drain everything; tree must end up empty and balanced throughout.
        for (int k : present) {
            assert(t.remove(k));
        }
        present.clear();
        assert(t.empty());
        assert(t.height() == -1);
        assert(t.in_order().empty());
        std::cout << "deletes: root/middle/leaf + drain OK\n";
    }

    // --- Empty-tree guards ---------------------------------------------------
    {
        AVL t;
        assert(t.empty());
        assert(!t.search(1));
        assert(!t.remove(1));
        assert(t.height() == -1);
        std::cout << "empty tree: guarded ops OK\n";
    }

    std::cout << "avl_tree: all tests passed\n";
    return 0;
}
