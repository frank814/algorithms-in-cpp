// bst: insert, search, delete, min/max, in-order, height
// duplicates are ignored

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

    // false if the key is already in
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

    // false if the key isn't there
    bool remove(int key) {
        Node* parent = nullptr;
        Node* cur = root_;
        while (cur != nullptr && cur->key != key) {
            parent = cur;
            cur = (key < cur->key) ? cur->left : cur->right;
        }
        if (cur == nullptr) return false;  // key not found

        // two children: copy the successor's key over
        if (cur->left != nullptr && cur->right != nullptr) {
            Node* succParent = cur;
            Node* succ = cur->right;
            while (succ->left != nullptr) {
                succParent = succ;
                succ = succ->left;
            }
            cur->key = succ->key;
            // successor has at most one child, unlink below
            parent = succParent;
            cur = succ;
        }

        // leaf or single child
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

    // needs a non-empty tree
    int min() const {
        assert(root_ != nullptr);
        Node* node = root_;
        while (node->left != nullptr) node = node->left;
        return node->key;
    }

    // needs a non-empty tree
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

    // empty tree counts as -1
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
    // in-order comes out sorted
    {
        BST t;
        std::vector<int> keys = {50, 30, 70, 20, 40, 60, 80, 45};
        for (int k : keys) assert(t.insert(k));

        std::vector<int> expected = {20, 30, 40, 45, 50, 60, 70, 80};
        assert(t.in_order() == expected);
        assert(t.min() == 20);
        assert(t.max() == 80);
        assert(t.height() == 3);  // tallest path is 50-30-40-45
        std::cout << "in-order: ";
        bool first = true;
        for (int k : t.in_order()) { std::cout << (first ? "" : " ") << k; first = false; }
        std::cout << '\n';
    }

    // search hits and misses
    {
        BST t;
        for (int k : {50, 30, 70, 20, 40, 60, 80}) assert(t.insert(k));
        for (int k : {50, 20, 80, 40}) assert(t.search(k));
        for (int k : {25, 55, 999, -1}) assert(!t.search(k));
    }

    // duplicates ignored
    {
        BST t;
        assert(t.insert(5));
        assert(!t.insert(5));
        assert(t.insert(4));
        assert(!t.insert(4));
        assert(t.in_order() == (std::vector<int>{4, 5}));
        std::cout << "duplicate policy: size stays 2\n";
    }

    // delete a leaf
    {
        BST t;
        for (int k : {50, 30, 70, 20}) assert(t.insert(k));
        assert(t.remove(20));  // leaf
        assert(!t.search(20));
        assert(t.in_order() == (std::vector<int>{30, 50, 70}));
    }

    // delete a node with one child
    {
        BST t;
        for (int k : {50, 30, 70, 20, 25}) assert(t.insert(k));
        assert(t.remove(20));  // 20 has only right child 25
        assert(!t.search(20));
        assert(t.search(25));
        assert(t.in_order() == (std::vector<int>{25, 30, 50, 70}));
    }

    // delete a node with two children
    {
        BST t;
        for (int k : {50, 30, 70, 20, 40, 60, 80}) assert(t.insert(k));
        assert(t.remove(30));  // successor is 40
        assert(!t.search(30));
        assert(t.in_order() == (std::vector<int>{20, 40, 50, 60, 70, 80}));

        // deeper successor this time
        BST t2;
        for (int k : {50, 30, 70, 20, 40, 60, 80, 65}) assert(t2.insert(k));
        assert(t2.remove(70));  // successor is 65
        assert(t2.in_order() == (std::vector<int>{20, 30, 40, 50, 60, 65, 80}));
    }

    // delete the root, then a missing key
    {
        BST t;
        for (int k : {50, 30, 70}) assert(t.insert(k));
        assert(t.remove(50));  // root with two children
        assert(t.in_order() == (std::vector<int>{30, 70}));
        assert(!t.remove(12345));
    }

    // empty tree
    {
        BST t;
        assert(t.empty());
        assert(t.height() == -1);
        assert(t.in_order().empty());
        assert(!t.search(1));
        assert(!t.remove(1));
        // min/max need a non-empty tree, skip them here
        std::cout << "empty tree: guarded ops OK\n";
    }

    // delete everything
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
