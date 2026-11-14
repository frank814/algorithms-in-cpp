// depth_first_search.cpp
// Depth-first search toolkit:
//   (a) recursive and iterative DFS over an adjacency-list graph;
//   (b) pre/in/post-order traversals of a binary tree (recursive);
//   (c) cycle detection in a directed graph (3-color DFS).
//
// Time complexity: O(V + E) for graph DFS and cycle detection,
// O(n) for tree traversals. Space: O(V) for visited sets, O(h) recursion.

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <stack>
#include <vector>

// ---------------------------------------------------------------------------
// (a) Graph DFS — recursive and iterative, both record discovery order.
// ---------------------------------------------------------------------------
using Graph = std::vector<std::vector<int>>;

// Iterative DFS from `start`, pushing vertices to `order` in discovery order.
// Handles disconnected graphs by iterating all vertices from the caller.
std::vector<int> dfs_iterative(const Graph& g, int start) {
    std::vector<int> order;
    std::vector<bool> visited(g.size(), false);
    std::stack<int> st;
    st.push(start);
    while (!st.empty()) {
        int u = st.top();
        st.pop();
        if (visited[u]) continue;
        visited[u] = true;
        order.push_back(u);
        // Push neighbours in reverse so the smallest index is visited first
        // (keeps discovery order identical to the recursive version).
        const auto& nbrs = g[u];
        for (auto it = nbrs.rbegin(); it != nbrs.rend(); ++it) {
            if (!visited[*it]) st.push(*it);
        }
    }
    return order;
}

// Recursive DFS helper: visits `u`, then its unvisited neighbours in order.
void dfs_recursive_visit(const Graph& g, int u,
                          std::vector<bool>& visited, std::vector<int>& order) {
    visited[u] = true;
    order.push_back(u);
    for (int v : g[u]) {
        if (!visited[v]) dfs_recursive_visit(g, v, visited, order);
    }
}

// Recursive DFS over the whole graph (covers every connected component).
std::vector<int> dfs_recursive(const Graph& g, int start) {
    std::vector<bool> visited(g.size(), false);
    std::vector<int> order;
    dfs_recursive_visit(g, start, visited, order);
    // Sweep remaining vertices so disconnected components are included.
    for (int u = 0; u < static_cast<int>(g.size()); ++u) {
        if (!visited[u]) dfs_recursive_visit(g, u, visited, order);
    }
    return order;
}

// Iterative DFS over the whole graph starting from vertex 0-style sweep.
std::vector<int> dfs_iterative_all(const Graph& g) {
    std::vector<int> order;
    std::vector<bool> visited(g.size(), false);
    for (int s = 0; s < static_cast<int>(g.size()); ++s) {
        if (visited[s]) continue;
        std::stack<int> st;
        st.push(s);
        while (!st.empty()) {
            int u = st.top();
            st.pop();
            if (visited[u]) continue;
            visited[u] = true;
            order.push_back(u);
            const auto& nbrs = g[u];
            for (auto it = nbrs.rbegin(); it != nbrs.rend(); ++it) {
                if (!visited[*it]) st.push(*it);
            }
        }
    }
    return order;
}

// ---------------------------------------------------------------------------
// (b) Binary tree traversals (recursive).
// ---------------------------------------------------------------------------
struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    explicit TreeNode(int v) : val(v), left(nullptr), right(nullptr) {}
};

void preorder(const TreeNode* n, std::vector<int>& out) {
    if (n == nullptr) return;
    out.push_back(n->val);
    preorder(n->left, out);
    preorder(n->right, out);
}

void inorder(const TreeNode* n, std::vector<int>& out) {
    if (n == nullptr) return;
    inorder(n->left, out);
    out.push_back(n->val);
    inorder(n->right, out);
}

void postorder(const TreeNode* n, std::vector<int>& out) {
    if (n == nullptr) return;
    postorder(n->left, out);
    postorder(n->right, out);
    out.push_back(n->val);
}

// Frees every node of a tree rooted at `n`.
void free_tree(TreeNode* n) {
    if (n == nullptr) return;
    free_tree(n->left);
    free_tree(n->right);
    delete n;
}

// ---------------------------------------------------------------------------
// (c) Cycle detection in a directed graph (DFS with white/gray/black colors).
// A back edge to a currently-gray vertex means a cycle exists.
// ---------------------------------------------------------------------------
bool has_cycle_directed(const Graph& g) {
    enum Color { White, Gray, Black };
    std::vector<Color> color(g.size(), White);

    std::function<bool(int)> visit = [&](int u) -> bool {
        color[u] = Gray;
        for (int v : g[u]) {
            if (color[v] == Gray) return true;   // back edge -> cycle
            if (color[v] == White && visit(v)) return true;
        }
        color[u] = Black;
        return false;
    };

    for (int u = 0; u < static_cast<int>(g.size()); ++u) {
        if (color[u] == White && visit(u)) return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
int main() {
    // --- (a) recursive vs iterative DFS produce identical order --------------
    {
        // Fixed graph, vertices 0..5:
        // 0 -> 1, 2 ; 1 -> 3 ; 2 -> 3, 4 ; 3 -> 5 ; 4 -> 5 ; 5 sink.
        Graph g(6);
        g[0] = {1, 2};
        g[1] = {3};
        g[2] = {3, 4};
        g[3] = {5};
        g[4] = {5};
        // g[5] = {}

        auto rec = dfs_recursive(g, 0);
        auto itr = dfs_iterative(g, 0);
        assert(rec == itr);
        assert(rec.front() == 0);
        assert(rec.size() == 6);  // all vertices reached
        std::cout << "DFS order: ";
        for (int v : rec) std::cout << v << ' ';
        std::cout << '\n';
    }

    // --- Disconnected graph: every component is visited ----------------------
    {
        Graph g(6);
        g[0] = {1};
        g[1] = {0};
        g[2] = {3};   // component {2,3}
        g[3] = {2};
        // 4 and 5 are isolated singletons.
        auto rec = dfs_recursive(g, 0);
        auto itr = dfs_iterative_all(g);
        assert(rec == itr);
        assert(rec.size() == 6);
        std::vector<bool> seen(6, false);
        for (int v : rec) seen[v] = true;
        for (int v = 0; v < 6; ++v) assert(seen[v]);
        std::cout << "disconnected graph visited all 6 vertices\n";
    }

    // --- Single-node graph ---------------------------------------------------
    {
        Graph g(1);
        assert(dfs_recursive(g, 0) == std::vector<int>{0});
        assert(dfs_iterative(g, 0) == std::vector<int>{0});
        assert(!has_cycle_directed(g));
    }

    // --- Tree traversals verified against each other -------------------------
    {
        // Tree shape:
        //          1
        //        /   .
        //       2     3
        //      / .     .
        //     4   5     6
        TreeNode* r = new TreeNode(1);
        r->left = new TreeNode(2);
        r->right = new TreeNode(3);
        r->left->left = new TreeNode(4);
        r->left->right = new TreeNode(5);
        r->right->right = new TreeNode(6);

        std::vector<int> pre, in, post;
        preorder(r, pre);
        inorder(r, in);
        postorder(r, post);

        assert((pre == std::vector<int>{1, 2, 4, 5, 3, 6}));
        assert((in == std::vector<int>{4, 2, 5, 1, 3, 6}));
        assert((post == std::vector<int>{4, 5, 2, 6, 3, 1}));

        // Cross-verification: all three contain each node exactly once and
        // agree on root-first / root-last structure.
        assert(pre.size() == in.size() && in.size() == post.size());
        assert(pre.front() == 1 && post.back() == 1 && post.front() == 4);
        // All three orders must be permutations of the same node set.
        std::vector<int> a = pre, b = in, c = post;
        std::sort(a.begin(), a.end());
        std::sort(b.begin(), b.end());
        std::sort(c.begin(), c.end());
        std::vector<int> nodes = {1, 2, 3, 4, 5, 6};
        assert(a == nodes && b == nodes && c == nodes);
        // Last node of post = root; in-order starts at the leftmost leaf (4)
        // and ends at the rightmost node (6).
        assert(in.front() == 4 && in.back() == 6);

        std::cout << "pre : 1 2 4 5 3 6\npost: 4 5 2 6 3 1\n";
        free_tree(r);
    }

    // --- Single-node tree traversals ----------------------------------------
    {
        TreeNode* r = new TreeNode(42);
        std::vector<int> pre, in, post;
        preorder(r, pre);
        inorder(r, in);
        postorder(r, post);
        assert((pre == std::vector<int>{42}));
        assert((in == std::vector<int>{42}));
        assert((post == std::vector<int>{42}));
        free_tree(r);
    }

    // --- Cycle detection: acyclic vs cyclic ---------------------------------
    {
        // Acyclic DAG.
        Graph dag(4);
        dag[0] = {1, 2};
        dag[1] = {3};
        dag[2] = {3};
        assert(!has_cycle_directed(dag));

        // With a back edge 3 -> 0.
        Graph cyc = dag;
        cyc[3] = {0};
        assert(has_cycle_directed(cyc));

        // Self-loop.
        Graph self(2);
        self[0] = {0};
        self[1] = {};
        assert(has_cycle_directed(self));

        // Cycle in a component not reachable from vertex 0.
        Graph hidden(4);
        hidden[0] = {1};
        hidden[1] = {};
        hidden[2] = {3};
        hidden[3] = {2};
        assert(has_cycle_directed(hidden));

        // Single node, no self-loop.
        Graph one(1);
        assert(!has_cycle_directed(one));

        std::cout << "cycle detection: DAG/cycle/self-loop/hidden cycle OK\n";
    }

    std::cout << "depth_first_search: all tests passed\n";
    return 0;
}
