// binary_search_tree.cpp
// Binary search tree (BST) supporting insert, search, delete (leaf / one child /
// two children via in-order successor), min/max, in-order traversal and height.
// Duplicate policy: duplicates are IGNORED (insert of an existing key returns
// false and leaves the tree unchanged).
//
// Time complexity (average / worst = skewed tree):
//   insert, search, delete, min, max : O(log n) / O(n)
//   in-order traversal, height       : O(n)
// Space complexity: O(n) for the nodes, O(h) recursion stack.

#include <cassert>
#include <functional>
#include <iostream>
#include <vector>

class BST {
public:
    struct Node {
        int key;
        Node* left;
        Node* right;
        explicit Node(int k) : key(k), left(nullptr), right(nullptr) {}
    };

    BST() = default;
    ~BST() { clear(root_); }

    BST(const BST&) = delete;
    BST& operator=(const BST&) = delete;

    // Inserts key; returns false if the key already exists.
    bool insert(int key) {
        Node** link = &root_;
        while (*link != nullptr) {
            Node* node = *link;
            if (key == node->key) return false;  // duplicates ignored
            link = (key < node->key) ? &node->left : &node->right;
        }
        *link = new Node(key);
        return true;
    }

    bool search(int key) const {
        for (Node* node = root_; node != nullptr; ) {
            if (key == node->key) return true;
            node = (key < node->key) ? node->left : node->right;
        }
        return false;
    }

    // Removes key if present; returns false when the key is not found.
    bool remove(int key) {
        Node* parent = nullptr;
        Node* cur = root_;
        while (cur != nullptr && cur->key != key) {
            parent = cur;
            cur = (key < cur->key) ? cur->left : cur->right;
        }
        if (cur == nullptr) return false;  // key not found

        // Case 3: two children -> replace with in-order successor.
        if (cur->left != nullptr && cur->right != nullptr) {
            Node* succParent = cur;
            Node* succ = cur->right;
            while (succ->left != nullptr) {
                succParent = succ;
                succ = succ->left;
            }
            cur->key = succ->key;
            // Now the successor node has at most one child; re-link below.
            parent = succParent;
            cur = succ;
        }

        // Cases 1 & 2: leaf or single child.
        Node* child = (cur->left != nullptr) ? cur->left : cur->right;
        if (parent == nullptr) {
            root_ = child;
        } else if (parent->left == cur) {
            parent->left = child;
        } else {
            parent->right = child;
        }
        delete cur;
        return true;
    }

    // Minimum key; requires a non-empty tree.
    int min() const {
        assert(root_ != nullptr);
        Node* node = root_;
        while (node->left != nullptr) node = node->left;
        return node->key;
    }

    // Maximum key; requires a non-empty tree.
    int max() const {
        assert(root_ != nullptr);
        Node* node = root_;
        while (node->right != nullptr) node = node->right;
        return node->key;
    }

    std::vector<int> in_order() const {
        std::vector<int> out;
        std::function<void(Node*)> walk = [&](Node* n) {
            if (n == nullptr) return;
            walk(n->left);
            out.push_back(n->key);
            walk(n->right);
        };
        walk(root_);
        return out;
    }

    // Height of an empty tree is defined as -1.
    int height() const { return heightOf(root_); }

    bool empty() const { return root_ == nullptr; }
    Node* root() const { return root_; }

private:
    Node* root_ = nullptr;

    static int heightOf(const Node* n) {
        if (n == nullptr) return -1;
        return 1 + std::max(heightOf(n->left), heightOf(n->right));
    }

    static void clear(Node* n) {
        if (n == nullptr) return;
        clear(n->left);
        clear(n->right);
        delete n;
    }
};

int main() {
    // --- Insert sequence -> in-order traversal is sorted ---------------------
    {
        BST t;
        std::vector<int> keys = {50, 30, 70, 20, 40, 60, 80, 45};
        for (int k : keys) assert(t.insert(k));

        std::vector<int> expected = {20, 30, 40, 45, 50, 60, 70, 80};
        assert(t.in_order() == expected);
        assert(t.min() == 20);
        assert(t.max() == 80);
        assert(t.height() == 3);  // 50 -> 30 -> 40 -> 45
        std::cout << "in-order: ";
        bool first = true;
        for (int k : t.in_order()) { std::cout << (first ? "" : " ") << k; first = false; }
        std::cout << '\n';
    }

    // --- Search hits and misses ---------------------------------------------
    {
        BST t;
        for (int k : {50, 30, 70, 20, 40, 60, 80}) assert(t.insert(k));
        for (int k : {50, 20, 80, 40}) assert(t.search(k));
        for (int k : {25, 55, 999, -1}) assert(!t.search(k));
    }

    // --- Duplicate policy: duplicates ignored -------------------------------
    {
        BST t;
        assert(t.insert(5));
        assert(!t.insert(5));
        assert(t.insert(4));
        assert(!t.insert(4));
        assert(t.in_order() == (std::vector<int>{4, 5}));
        std::cout << "duplicate policy: size stays 2\n";
    }

    // --- Delete case 1: leaf -------------------------------------------------
    {
        BST t;
        for (int k : {50, 30, 70, 20}) assert(t.insert(k));
        assert(t.remove(20));  // leaf
        assert(!t.search(20));
        assert(t.in_order() == (std::vector<int>{30, 50, 70}));
    }

    // --- Delete case 2: one child -------------------------------------------
    {
        BST t;
        for (int k : {50, 30, 70, 20, 25}) assert(t.insert(k));
        assert(t.remove(20));  // 20 has only right child 25
        assert(!t.search(20));
        assert(t.search(25));
        assert(t.in_order() == (std::vector<int>{25, 30, 50, 70}));
    }

    // --- Delete case 3: two children (in-order successor) -------------------
    {
        BST t;
        for (int k : {50, 30, 70, 20, 40, 60, 80}) assert(t.insert(k));
        assert(t.remove(30));  // children 20, 40 -> successor 40
        assert(!t.search(30));
        assert(t.in_order() == (std::vector<int>{20, 40, 50, 60, 70, 80}));

        // Successor that is deeper in the right subtree.
        BST t2;
        for (int k : {50, 30, 70, 20, 40, 60, 80, 65}) assert(t2.insert(k));
        assert(t2.remove(70));  // children 60, 80 -> successor 65
        assert(t2.in_order() == (std::vector<int>{20, 30, 40, 50, 60, 65, 80}));
    }

    // --- Delete root, delete missing key -------------------------------------
    {
        BST t;
        for (int k : {50, 30, 70}) assert(t.insert(k));
        assert(t.remove(50));  // root with two children
        assert(t.in_order() == (std::vector<int>{30, 70}));
        assert(!t.remove(12345));
    }

    // --- Empty-tree operations are guarded -----------------------------------
    {
        BST t;
        assert(t.empty());
        assert(t.height() == -1);
        assert(t.in_order().empty());
        assert(!t.search(1));
        assert(!t.remove(1));
        // min()/max() assert non-empty, so only called on non-empty trees.
        std::cout << "empty tree: guarded ops OK\n";
    }

    // --- Full delete until empty --------------------------------------------
    {
        BST t;
        std::vector<int> keys = {40, 20, 60, 10, 30, 50, 70, 25, 35};
        for (int k : keys) assert(t.insert(k));
        for (int k : keys) assert(t.remove(k));
        assert(t.empty());
        assert(t.height() == -1);
    }

    std::cout << "binary_search_tree: all tests passed\n";
    return 0;
}
