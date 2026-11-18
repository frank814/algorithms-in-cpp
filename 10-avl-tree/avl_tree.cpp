// avl tree - insert/delete with rotations, keeps height diff <= 1
// duplicates ignored

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
        int height;  // height in edges, leaf = 0
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

    // checks bst order, balance and stored heights, returns height
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

    // rebalance after a child changed
    static Node* rebalance(Node* n) {
        update(n);
        int b = balanceOf(n);
        if (b > 1) {                      // left-heavy
            if (balanceOf(n->left) < 0) { // left-right case
                n->left = rotateLeft(n->left);
            }
            return rotateRight(n);        // left-left case
        }
        if (b < -1) {                     // right-heavy
            if (balanceOf(n->right) > 0) { // right-left case
                n->right = rotateRight(n->right);
            }
            return rotateLeft(n);         // right-right case
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

    // pulls the in-order successor out of the right subtree, returns its key
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
                // two children: swap in the successor's key
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

int main() {
    // check invariants and sorted order after each op
    auto check = [](AVL& t, const std::vector<int>& expected) {
        AVL::verify(t.root(), LONG_MIN, LONG_MAX);
        assert(t.in_order() == expected);
    };

    // insert sequence
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

    // all four rotation cases
    {
        // left-left: 30 20 10
        AVL ll;
        for (int k : {30, 20, 10}) assert(ll.insert(k));
        check(ll, {10, 20, 30});
        assert(ll.root()->key == 20);

        // right-right: 10 20 30
        AVL rr;
        for (int k : {10, 20, 30}) assert(rr.insert(k));
        check(rr, {10, 20, 30});
        assert(rr.root()->key == 20);

        // left-right: 30 10 20
        AVL lr;
        for (int k : {30, 10, 20}) assert(lr.insert(k));
        check(lr, {10, 20, 30});
        assert(lr.root()->key == 20);

        // right-left: 10 30 20
        AVL rl;
        for (int k : {10, 30, 20}) assert(rl.insert(k));
        check(rl, {10, 20, 30});
        assert(rl.root()->key == 20);

        std::cout << "rotation cases: LL/RR/LR/RL all balanced\n";
    }

    // sorted insert stays shallow
    {
        AVL t;
        const int n = 1000;
        for (int k = 0; k < n; ++k) assert(t.insert(k));
        int h = t.height();
        double bound = 1.44 * std::log2(static_cast<double>(n + 2));
        std::cout << "sorted insert n=" << n << " height=" << h
                  << " bound=" << bound << '\n';
        assert(static_cast<double>(h) <= bound);
        // plain BST would be height n-1
        assert(h < n - 1);

        std::vector<int> expected(n);
        for (int k = 0; k < n; ++k) expected[k] = k;
        check(t, expected);
    }

    // random workload
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

    // root / middle / leaf deletes
    {
        AVL t;
        std::vector<int> keys = {40, 20, 60, 10, 30, 50, 70, 5, 15, 25, 35};
        std::vector<int> present = keys;
        std::sort(present.begin(), present.end());
        for (int k : keys) assert(t.insert(k));
        check(t, present);

        // leaf delete (5), middle (20), root (40)
        for (int k : {5, 20, 40}) {
            assert(t.remove(k));
            present.erase(std::find(present.begin(), present.end(), k));
            check(t, present);
        }
        assert(!t.search(40));
        assert(!t.remove(40));   // already gone
        assert(!t.remove(999));

        // drain the rest
        for (int k : present) {
            assert(t.remove(k));
        }
        present.clear();
        assert(t.empty());
        assert(t.height() == -1);
        assert(t.in_order().empty());
        std::cout << "deletes: root/middle/leaf + drain OK\n";
    }

    // empty tree
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
