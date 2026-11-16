// splay_tree.cpp
// Splay tree: a self-adjusting BST where every search/insert/delete splashes
// ("splays") the accessed node to the root using zig, zig-zig and zig-zag
// rotations. No height/balance metadata is stored; the amortised cost of each
// operation is O(log n).
// Duplicate policy: inserting an existing key splays it to the root and
// returns false (the key set is unchanged).
//
// Time complexity: O(log n) amortised per operation (worst case O(n)),
// O(n) for in-order traversal. Space: O(n), recursion depth O(n) worst case
// but O(log n) amortised.

#include <algorithm>
#include <cassert>
#include <climits>
#include <iostream>
#include <vector>

class SplayTree {
public:
    struct Node {
        int key;
        Node* left;
        Node* right;
        explicit Node(int k) : key(k), left(nullptr), right(nullptr) {}
    };

    SplayTree() = default;
    ~SplayTree() { clear(root_); }

    SplayTree(const SplayTree&) = delete;
    SplayTree& operator=(const SplayTree&) = delete;

    // Splays the node holding `key` (or the last node reached) to the root.
    // Zig:     rotate at the root.
    // Zig-zig: child and parent both on the same side -> rotate twice.
    // Zig-zag: child and parent on opposite sides -> rotate twice, crossed.
    Node* splay(Node* t, int key) const {
        if (t == nullptr) return nullptr;
        if (key < t->key) {
            if (t->left == nullptr) return t;   // key absent, stop here
            t->left = splay(t->left, key);
            return rotateRight(t);              // zig / zig-zig / zig-zag
        }
        if (key > t->key) {
            if (t->right == nullptr) return t;
            t->right = splay(t->right, key);
            return rotateLeft(t);
        }
        return t;  // found: already the sub-root
    }

    // Returns true if inserted; false if the key already existed (in which
    // case the existing node ends up at the root).
    bool insert(int key) {
        if (root_ == nullptr) {
            root_ = new Node(key);
            return true;
        }
        root_ = splay(root_, key);
        if (root_->key == key) return false;  // duplicate ignored

        Node* n = new Node(key);
        if (key < root_->key) {
            n->left = root_->left;
            n->right = root_;
            root_->left = nullptr;
        } else {
            n->right = root_->right;
            n->left = root_;
            root_->right = nullptr;
        }
        root_ = n;
        return true;
    }

    // Searches for `key`; on success the node is at the root.
    bool search(int key) {
        root_ = splay(root_, key);
        return root_ != nullptr && root_->key == key;
    }

    // Removes `key` if present; the successor structure is re-rooted by
    // splaying the maximum of the left subtree.
    bool remove(int key) {
        if (root_ == nullptr) return false;
        root_ = splay(root_, key);
        if (root_->key != key) return false;  // not found

        Node* doomed = root_;
        Node* left = doomed->left;
        Node* right = doomed->right;
        if (left == nullptr) {
            root_ = right;
        } else {
            // Splay the largest key of the left subtree to its top; every key
            // there is smaller than `key`, so searching for `key` lands on the
            // maximum.
            left = splay(left, key);
            left->right = right;
            root_ = left;
        }
        delete doomed;
        return true;
    }

    std::vector<int> in_order() const {
        std::vector<int> out;
        walk(root_, out);
        return out;
    }

    bool empty() const { return root_ == nullptr; }
    Node* root() const { return root_; }

    // Recursively checks the BST key ordering; returns the node count.
    static int check_bst(const Node* n, long lo, long hi) {
        if (n == nullptr) return 0;
        assert(lo < n->key && n->key < hi);
        return 1 + check_bst(n->left, lo, n->key) + check_bst(n->right, n->key, hi);
    }

private:
    Node* root_ = nullptr;

    // Right rotation (zig):        y          x
    //                           x     C ->  A   y
    //                         A   B           B  C
    static Node* rotateRight(Node* y) {
        Node* x = y->left;
        if (x == nullptr) return y;
        y->left = x->right;
        x->right = y;
        return x;
    }

    static Node* rotateLeft(Node* x) {
        Node* y = x->right;
        if (y == nullptr) return x;
        x->right = y->left;
        y->left = x;
        return y;
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
    // Helper: full invariant check (BST order + in-order == expected set).
    auto check = [](SplayTree& t, const std::vector<int>& expected) {
        SplayTree::check_bst(t.root(), LONG_MIN, LONG_MAX);
        assert(t.in_order() == expected);
    };

    // --- Inserts keep the in-order sequence sorted --------------------------
    {
        SplayTree t;
        std::vector<int> keys = {50, 30, 70, 20, 40, 60, 80, 45};
        std::vector<int> sorted;
        for (int k : keys) {
            assert(t.insert(k));
            sorted.push_back(k);
            std::sort(sorted.begin(), sorted.end());
            check(t, sorted);
        }
        std::cout << "inserts: in-order sorted, BST property holds\n";
    }

    // --- Every successful search ends with the node at the root -------------
    {
        SplayTree t;
        for (int k : {50, 30, 70, 20, 40, 60, 80}) assert(t.insert(k));
        std::vector<int> sorted = {20, 30, 40, 50, 60, 70, 80};
        for (int k : sorted) {
            assert(t.search(k));
            assert(t.root()->key == k);  // splayed to root
            check(t, sorted);
        }
        // Failed searches also splay some node to the root without changing
        // the key set.
        for (int k : {-5, 25, 55, 100}) {
            assert(!t.search(k));
            check(t, sorted);
        }
        std::cout << "search: accessed node always at root\n";
    }

    // --- Duplicate insert policy --------------------------------------------
    {
        SplayTree t;
        assert(t.insert(10));
        assert(t.insert(5));
        assert(t.insert(15));
        assert(!t.insert(10));            // duplicate rejected
        assert(t.root()->key == 10);      // ...and splayed to the root
        check(t, {5, 10, 15});
        std::cout << "duplicates: rejected, existing node at root\n";
    }

    // --- Delete of root and of a non-root node ------------------------------
    {
        SplayTree t;
        std::vector<int> keys = {40, 20, 60, 10, 30, 50, 70};
        std::vector<int> present = keys;
        std::sort(present.begin(), present.end());
        for (int k : keys) assert(t.insert(k));

        // Delete the root: search splays 40 to the root, then it is removed.
        assert(t.search(40));
        assert(t.remove(40));
        present.erase(std::find(present.begin(), present.end(), 40));
        check(t, present);
        assert(!t.search(40));

        // Delete a leaf-ish node (10).
        assert(t.remove(10));
        present.erase(std::find(present.begin(), present.end(), 10));
        check(t, present);

        // Delete an internal node with two children (60).
        assert(t.remove(60));
        present.erase(std::find(present.begin(), present.end(), 60));
        check(t, present);

        // Deleting a missing key fails and keeps the tree intact.
        assert(!t.remove(999));
        check(t, present);

        std::cout << "delete: root / leaf / internal node OK\n";
    }

    // --- Delete root repeatedly until empty ---------------------------------
    {
        SplayTree t;
        std::vector<int> keys = {30, 10, 20, 40, 50};
        for (int k : keys) assert(t.insert(k));
        for (size_t i = keys.size(); i-- > 0; ) {
            int r = t.root()->key;
            assert(t.remove(r));  // always delete whatever sits at the root
        }
        assert(t.empty());
        assert(t.in_order().empty());
        assert(!t.search(1));
        assert(!t.remove(1));
        assert(t.insert(7));  // still usable after becoming empty
        assert((t.in_order() == std::vector<int>{7}));
        std::cout << "drain by root-delete: tree empties and recovers\n";
    }

    // --- Repeated-access sequence (path compression behaviour) --------------
    {
        SplayTree t;
        for (int k = 1; k <= 31; ++k) assert(t.insert(k));
        // Access the same key repeatedly: it stays at the root.
        for (int i = 0; i < 5; ++i) {
            assert(t.search(16));
            assert(t.root()->key == 16);
        }
        // Alternate two hot keys: each search moves that key to the root.
        for (int i = 0; i < 4; ++i) {
            assert(t.search(5));
            assert(t.root()->key == 5);
            assert(t.search(28));
            assert(t.root()->key == 28);
        }
        std::vector<int> expected;
        for (int k = 1; k <= 31; ++k) expected.push_back(k);
        check(t, expected);
        std::cout << "repeated access: hot keys stay/return at root\n";
    }

    // --- Ascending inserts (worst case for a plain BST) ---------------------
    {
        SplayTree t;
        std::vector<int> expected;
        for (int k = 0; k < 100; ++k) {
            assert(t.insert(k));
            expected.push_back(k);
            check(t, expected);
        }
        assert(t.search(50));
        assert(t.root()->key == 50);
        check(t, expected);
        std::cout << "ascending inserts: structure valid, 50 splayed\n";
    }

    std::cout << "splay_tree: all tests passed\n";
    return 0;
}
