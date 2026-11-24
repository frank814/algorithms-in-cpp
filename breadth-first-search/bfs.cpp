// bfs on an adjacency list, shortest path when every edge costs 1
// missing or unreachable endpoints give distance -1

#include <cassert>
#include <iostream>
#include <map>
#include <queue>
#include <vector>

class Graph {
public:
    explicit Graph(bool directed = false) : directed_(directed) {}

    // standalone node, no edges
    void add_node(int n) { adjacency_[n]; }

    // both directions unless the graph is directed
    void add_edge(int u, int v) {
        adjacency_[u].push_back(v);
        if (!directed_) {
            adjacency_[v].push_back(u);
        } else {
            adjacency_[v];  // still counts v as a node
        }
    }

    bool has_node(int n) const { return adjacency_.count(n) != 0; }

    // unknown nodes just give an empty list
    const std::vector<int>& neighbors(int n) const {
        static const std::vector<int> empty;
        auto it = adjacency_.find(n);
        return it == adjacency_.end() ? empty : it->second;
    }

    std::size_t node_count() const { return adjacency_.size(); }
    bool is_directed() const { return directed_; }

private:
    bool directed_;
    std::map<int, std::vector<int>> adjacency_;
};

struct BfsResult {
    std::vector<int> path;  // empty when unreachable
    int distance = -1;      // -1 when unreachable
};

// shortest path, every edge has cost 1
BfsResult bfs_shortest_path(const Graph& g, int src, int dst) {
    BfsResult result;
    // bad endpoint, nothing to search
    if (!g.has_node(src) || !g.has_node(dst)) {
        return result;
    }

    std::map<int, int> parent;  // visited set + path reconstruction
    std::queue<int> frontier;
    parent[src] = src;  // src is its own parent
    frontier.push(src);

    while (!frontier.empty()) {
        const int u = frontier.front();
        frontier.pop();
        if (u == dst) {
            break;  // first time we reach dst, so it is shortest
        }
        for (int v : g.neighbors(u)) {
            if (parent.find(v) == parent.end()) {
                parent[v] = u;
                frontier.push(v);
            }
        }
    }

    if (parent.find(dst) == parent.end()) {
        return result;  // no route to dst
    }

    // walk parents back to src, then reverse
    int cur = dst;
    result.path.push_back(cur);
    while (cur != src) {
        cur = parent.at(cur);
        result.path.push_back(cur);
    }
    for (std::size_t i = 0, j = result.path.size(); i + 1 < j; ++i, --j) {
        std::swap(result.path[i], result.path[j - 1]);
    }
    result.distance = static_cast<int>(result.path.size()) - 1;
    return result;
}

// checks every hop is a real edge (test helper)
bool is_valid_path(const Graph& g, const std::vector<int>& path, int src,
                   int dst) {
    if (path.empty()) {
        return false;
    }
    if (path.front() != src || path.back() != dst) {
        return false;
    }
    for (std::size_t i = 0; i + 1 < path.size(); ++i) {
        const std::vector<int>& nbrs = g.neighbors(path[i]);
        bool linked = false;
        for (int v : nbrs) {
            if (v == path[i + 1]) {
                linked = true;
                break;
            }
        }
        if (!linked) {
            return false;
        }
    }
    return true;
}

static void run_tests() {
    // path to self
    Graph self;
    self.add_node(1);
    BfsResult r = bfs_shortest_path(self, 1, 1);
    assert(r.distance == 0);
    assert((r.path == std::vector<int>{1}));

    // still 0 inside a bigger graph
    Graph chain;
    chain.add_edge(1, 2);
    chain.add_edge(2, 3);
    chain.add_edge(3, 4);
    r = bfs_shortest_path(chain, 2, 2);
    assert(r.distance == 0);
    assert((r.path == std::vector<int>{2}));

    // chain graph, one route through it
    r = bfs_shortest_path(chain, 1, 4);
    assert(r.distance == 3);
    assert((r.path == std::vector<int>{1, 2, 3, 4}));
    assert(is_valid_path(chain, r.path, 1, 4));
    r = bfs_shortest_path(chain, 3, 1);  // undirected, works backwards
    assert(r.distance == 2);
    assert((r.path == std::vector<int>{3, 2, 1}));

    // two components, can't cross over
    Graph two_components;
    two_components.add_edge(1, 2);
    two_components.add_edge(3, 4);
    r = bfs_shortest_path(two_components, 1, 2);
    assert(r.distance == 1);
    assert((r.path == std::vector<int>{1, 2}));
    r = bfs_shortest_path(two_components, 1, 3);  // across components
    assert(r.distance == -1);
    assert(r.path.empty());
    r = bfs_shortest_path(two_components, 4, 1);
    assert(r.distance == -1);
    assert(r.path.empty());

    // diamond, two equal routes
    Graph diamond;
    diamond.add_edge(0, 1);
    diamond.add_edge(0, 2);
    diamond.add_edge(1, 3);
    diamond.add_edge(2, 3);
    diamond.add_edge(3, 4);  // tail beyond the diamond
    r = bfs_shortest_path(diamond, 0, 3);
    assert(r.distance == 2);  // either way is 2 hops
    assert(r.path.size() == 3);
    assert(r.path.front() == 0 && r.path.back() == 3);
    assert(is_valid_path(diamond, r.path, 0, 3));
    r = bfs_shortest_path(diamond, 0, 4);
    assert(r.distance == 3);
    assert(is_valid_path(diamond, r.path, 0, 4));
    r = bfs_shortest_path(diamond, 3, 0);
    assert(r.distance == 2);  // two hops going back
    assert(is_valid_path(diamond, r.path, 3, 0));

    // short hop beats the long way round
    Graph detour;
    detour.add_edge(0, 3);  // direct hop
    detour.add_edge(0, 1);
    detour.add_edge(1, 2);
    detour.add_edge(2, 3);  // three-hop alternative
    r = bfs_shortest_path(detour, 0, 3);
    assert(r.distance == 1);
    assert((r.path == std::vector<int>{0, 3}));

    // directed edges only go one way
    Graph directed(true);
    directed.add_edge(5, 6);
    directed.add_edge(6, 7);
    r = bfs_shortest_path(directed, 5, 7);
    assert(r.distance == 2);
    assert((r.path == std::vector<int>{5, 6, 7}));
    r = bfs_shortest_path(directed, 7, 5);  // no backward edges
    assert(r.distance == -1);
    assert(r.path.empty());

    // node not in the graph
    Graph small;
    small.add_edge(10, 20);
    r = bfs_shortest_path(small, 10, 99);   // unknown target
    assert(r.distance == -1);
    assert(r.path.empty());
    r = bfs_shortest_path(small, 99, 10);   // unknown source
    assert(r.distance == -1);
    assert(r.path.empty());
    r = bfs_shortest_path(small, 98, 99);   // both unknown
    assert(r.distance == -1);
    assert(r.path.empty());
    r = bfs_shortest_path(small, 99, 99);   // self path needs a real node
    assert(r.distance == -1);
    assert(r.path.empty());

    // empty graph
    Graph empty_graph;
    r = bfs_shortest_path(empty_graph, 0, 0);
    assert(r.distance == -1);
    assert(r.path.empty());

    // negative ids
    Graph signed_nodes;
    signed_nodes.add_edge(-4, -1);
    signed_nodes.add_edge(-1, 2);
    r = bfs_shortest_path(signed_nodes, -4, 2);
    assert(r.distance == 2);
    assert((r.path == std::vector<int>{-4, -1, 2}));
    // -1 works fine as a source too
    r = bfs_shortest_path(signed_nodes, -1, -4);
    assert(r.distance == 1);
    assert((r.path == std::vector<int>{-1, -4}));
}

void print_query(const Graph& g, int src, int dst) {
    const BfsResult r = bfs_shortest_path(g, src, dst);
    std::cout << src << " -> " << dst << ": distance=" << r.distance
              << "  path=[";
    for (std::size_t i = 0; i < r.path.size(); ++i) {
        std::cout << (i == 0 ? "" : ", ") << r.path[i];
    }
    std::cout << "]\n";
}

int main() {
    run_tests();
    std::cout << "All BFS tests passed.\n\n";

    // demo: diamond plus an island
    Graph demo;
    demo.add_edge(0, 1);
    demo.add_edge(0, 2);
    demo.add_edge(1, 3);
    demo.add_edge(2, 3);
    demo.add_edge(3, 4);
    demo.add_edge(7, 8);  // disconnected island

    std::cout << "diamond graph (0..4) + island (7-8):\n";
    print_query(demo, 0, 4);
    print_query(demo, 4, 0);
    print_query(demo, 0, 3);
    print_query(demo, 0, 0);
    print_query(demo, 0, 8);  // unreachable component
    print_query(demo, 0, 42); // node not in graph
    return 0;
}
