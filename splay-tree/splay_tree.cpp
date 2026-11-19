// splay tree - every access splays the touched node up to the root
// inserting an existing key just splays it and returns false

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

    // splays key (or the last node reached) to the root
    // zig at the root, zig-zig same side twice, zig-zag crossed
    Node* splay(Node* t, int key) const {
        if (t == nullptr) return nullptr;
        if (key < t->key) {
            if (t->left == nullptr) return t;   // key absent, stop here
            t->left = splay(t->left, key);
            return rotateRight(t);
        }
        if (key > t->key) {
            if (t->right == nullptr) return t;
            t->right = splay(t->right, key);
            return rotateLeft(t);
        }
        return t;  // found
    }

    // false if the key existed (its node ends up at the root)
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

    // on success the found node is at the root
    bool search(int key) {
        root_ = splay(root_, key);
        return root_ != nullptr && root_->key == key;
    }

    // removes key if present, re-rooting on the left subtree's max
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
            // everything left of key is smaller, so the splay lands on the max
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

    // bst order check, returns node count
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

int main() {
    // bst order + in-order matches expected
    auto check = [](SplayTree& t, const std::vector<int>& expected) {
        SplayTree::check_bst(t.root(), LONG_MIN, LONG_MAX);
        assert(t.in_order() == expected);
    };

    // inserts stay sorted in-order
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

    // successful search ends with node at root
    {
        SplayTree t;
        for (int k : {50, 30, 70, 20, 40, 60, 80}) assert(t.insert(k));
        std::vector<int> sorted = {20, 30, 40, 50, 60, 70, 80};
        for (int k : sorted) {
            assert(t.search(k));
            assert(t.root()->key == k);  // splayed to root
            check(t, sorted);
        }
        // failed search still splays something, key set unchanged
        for (int k : {-5, 25, 55, 100}) {
            assert(!t.search(k));
            check(t, sorted);
        }
        std::cout << "search: accessed node always at root\n";
    }

    // duplicate insert policy
    {
        SplayTree t;
        assert(t.insert(10));
        assert(t.insert(5));
        assert(t.insert(15));
        assert(!t.insert(10));            // duplicate rejected
        assert(t.root()->key == 10);      // now at root
        check(t, {5, 10, 15});
        std::cout << "duplicates: rejected, existing node at root\n";
    }

    // delete root / leaf / internal
    {
        SplayTree t;
        std::vector<int> keys = {40, 20, 60, 10, 30, 50, 70};
        std::vector<int> present = keys;
        std::sort(present.begin(), present.end());
        for (int k : keys) assert(t.insert(k));

        // search splays 40 up first, then it gets removed
        assert(t.search(40));
        assert(t.remove(40));
        present.erase(std::find(present.begin(), present.end(), 40));
        check(t, present);
        assert(!t.search(40));

        // leaf-ish node
        assert(t.remove(10));
        present.erase(std::find(present.begin(), present.end(), 10));
        check(t, present);

        // internal node, two children
        assert(t.remove(60));
        present.erase(std::find(present.begin(), present.end(), 60));
        check(t, present);

        // missing key: fail, tree unchanged
        assert(!t.remove(999));
        check(t, present);

        std::cout << "delete: root / leaf / internal node OK\n";
    }

    // keep deleting the root until empty
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

    // repeated access (path compression behaviour)
    {
        SplayTree t;
        for (int k = 1; k <= 31; ++k) assert(t.insert(k));
        // same key over and over: stays at root
        for (int i = 0; i < 5; ++i) {
            assert(t.search(16));
            assert(t.root()->key == 16);
        }
        // two hot keys take turns at the root
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

    // ascending inserts
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
