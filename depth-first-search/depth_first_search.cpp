// dfs (recursive + iterative), tree traversals, directed cycle check

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <stack>
#include <vector>

using Graph = std::vector<std::vector<int>>;

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
        // reverse push keeps order the same as recursive
        const auto& nbrs = g[u];
        for (auto it = nbrs.rbegin(); it != nbrs.rend(); ++it) {
            if (!visited[*it]) st.push(*it);
        }
    }
    return order;
}

void dfs_recursive_visit(const Graph& g, int u,
                          std::vector<bool>& visited, std::vector<int>& order) {
    visited[u] = true;
    order.push_back(u);
    for (int v : g[u]) {
        if (!visited[v]) dfs_recursive_visit(g, v, visited, order);
    }
}

std::vector<int> dfs_recursive(const Graph& g, int start) {
    std::vector<bool> visited(g.size(), false);
    std::vector<int> order;
    dfs_recursive_visit(g, start, visited, order);
    // pick up whatever the first call missed
    for (int u = 0; u < static_cast<int>(g.size()); ++u) {
        if (!visited[u]) dfs_recursive_visit(g, u, visited, order);
    }
    return order;
}

// same full sweep, iterative
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

void free_tree(TreeNode* n) {
    if (n == nullptr) return;
    free_tree(n->left);
    free_tree(n->right);
    delete n;
}

// 3-color dfs, a back edge to a gray vertex means cycle
bool has_cycle_directed(const Graph& g) {
    enum Color { White, Gray, Black };
    std::vector<Color> color(g.size(), White);

    std::function<bool(int)> visit = [&](int u) -> bool {
        color[u] = Gray;
        for (int v : g[u]) {
            if (color[v] == Gray) return true;   // back edge means cycle
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

int main() {
    // recursive vs iterative give the same order
    {
        // 0 to 1,2; 1 to 3; 2 to 3,4; 3 and 4 to 5; 5 is the sink
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

    // disconnected graph
    {
        Graph g(6);
        g[0] = {1};
        g[1] = {0};
        g[2] = {3};   // component {2,3}
        g[3] = {2};
        // 4 and 5 are isolated
        auto rec = dfs_recursive(g, 0);
        auto itr = dfs_iterative_all(g);
        assert(rec == itr);
        assert(rec.size() == 6);
        std::vector<bool> seen(6, false);
        for (int v : rec) seen[v] = true;
        for (int v = 0; v < 6; ++v) assert(seen[v]);
        std::cout << "disconnected graph visited all 6 vertices\n";
    }

    // single node graph
    {
        Graph g(1);
        assert(dfs_recursive(g, 0) == std::vector<int>{0});
        assert(dfs_iterative(g, 0) == std::vector<int>{0});
        assert(!has_cycle_directed(g));
    }

    // tree traversals
    {
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

        // root first in pre, last in post
        assert(pre.size() == in.size() && in.size() == post.size());
        assert(pre.front() == 1 && post.back() == 1 && post.front() == 4);
        // same nodes in all three orders
        std::vector<int> a = pre, b = in, c = post;
        std::sort(a.begin(), a.end());
        std::sort(b.begin(), b.end());
        std::sort(c.begin(), c.end());
        std::vector<int> nodes = {1, 2, 3, 4, 5, 6};
        assert(a == nodes && b == nodes && c == nodes);
        // in-order starts at the leftmost node
        assert(in.front() == 4 && in.back() == 6);

        std::cout << "pre : 1 2 4 5 3 6\npost: 4 5 2 6 3 1\n";
        free_tree(r);
    }

    // single node tree
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

    // cycle detection
    {
        // DAG, no cycle
        Graph dag(4);
        dag[0] = {1, 2};
        dag[1] = {3};
        dag[2] = {3};
        assert(!has_cycle_directed(dag));

        // add a back edge 3 to 0
        Graph cyc = dag;
        cyc[3] = {0};
        assert(has_cycle_directed(cyc));

        // self loop
        Graph self(2);
        self[0] = {0};
        self[1] = {};
        assert(has_cycle_directed(self));

        // cycle in a part of the graph not reachable from 0
        Graph hidden(4);
        hidden[0] = {1};
        hidden[1] = {};
        hidden[2] = {3};
        hidden[3] = {2};
        assert(has_cycle_directed(hidden));

        // single node, no self loop
        Graph one(1);
        assert(!has_cycle_directed(one));

        std::cout << "cycle detection: DAG/cycle/self-loop/hidden cycle OK\n";
    }

    std::cout << "depth_first_search: all tests passed\n";
    return 0;
}
