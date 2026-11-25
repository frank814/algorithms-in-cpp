// dijkstra single-source shortest paths, binary heap
// needs non-negative weights; unreachable gets dist -1

#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

constexpr long long kInf = std::numeric_limits<long long>::max() / 4;

struct Edge {
    int to;
    long long weight;
};

using AdjList = std::vector<std::vector<Edge>>;

struct ShortestPaths {
    std::vector<long long> dist;   // -1 if unreachable
    std::vector<int> parent;       // predecessor, -1 if none
};

ShortestPaths dijkstra(const AdjList& graph, int source) {
    const int n = static_cast<int>(graph.size());
    if (source < 0 || source >= n) {
        throw std::invalid_argument("source vertex out of range");
    }
    for (const auto& edges : graph) {
        for (const Edge& e : edges) {
            if (e.weight < 0) {
                throw std::invalid_argument("negative edge weight");
            }
        }
    }

    std::vector<long long> dist(n, kInf);
    std::vector<int> parent(n, -1);
    dist[source] = 0;

    // min-heap of (dist, vertex)
    using Entry = std::pair<long long, int>;
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> pq;
    pq.push({0, source});

    while (!pq.empty()) {
        const auto [d, u] = pq.top();
        pq.pop();
        if (d > dist[u]) {
            continue;  // stale entry
        }
        for (const Edge& e : graph[u]) {
            const long long nd = d + e.weight;
            if (nd < dist[e.to]) {
                dist[e.to] = nd;
                parent[e.to] = u;
                pq.push({nd, e.to});
            }
        }
    }

    ShortestPaths result;
    result.dist.resize(n);
    result.parent = std::move(parent);
    for (int v = 0; v < n; ++v) {
        result.dist[v] = (dist[v] >= kInf) ? -1 : dist[v];
    }
    return result;
}

// walk parents back from target; empty if unreachable
std::vector<int> reconstruct_path(const ShortestPaths& sp, int source, int target) {
    std::vector<int> path;
    if (sp.dist[target] < 0) {
        return path;
    }
    for (int v = target; v != -1; v = sp.parent[v]) {
        path.push_back(v);
        if (v == source) {
            break;
        }
    }
    if (path.back() != source) {
        return {};  // doesn't reach source
    }
    std::reverse(path.begin(), path.end());
    return path;
}

}  // namespace

int main() {
    // test graph, edges: 0-1(4) 0-2(1) 2-1(2) 1-3(1) 2-3(5) 3-4(3)
    // dist from 0: 0 3 1 4 7; path to 4: 0 2 1 3 4
    AdjList g(5);
    auto add = [&g](int u, int v, long long w) {
        g[u].push_back({v, w});
    };
    add(0, 1, 4);
    add(0, 2, 1);
    add(2, 1, 2);
    add(1, 3, 1);
    add(2, 3, 5);
    add(3, 4, 3);

    ShortestPaths sp = dijkstra(g, 0);
    assert(sp.dist[0] == 0);
    assert(sp.dist[1] == 3);
    assert(sp.dist[2] == 1);
    assert(sp.dist[3] == 4);
    assert(sp.dist[4] == 7);
    assert(reconstruct_path(sp, 0, 4) == std::vector<int>({0, 2, 1, 3, 4}));
    assert(reconstruct_path(sp, 0, 0) == std::vector<int>({0}));

    // unreachable node
    AdjList g2(3);
    g2[0].push_back({1, 2});
    // vertex 2 isolated
    ShortestPaths sp2 = dijkstra(g2, 0);
    assert(sp2.dist[1] == 2);
    assert(sp2.dist[2] == -1);
    assert(reconstruct_path(sp2, 0, 2).empty());

    // single node
    AdjList g3(1);
    ShortestPaths sp3 = dijkstra(g3, 0);
    assert(sp3.dist[0] == 0);

    // zero-weight edges
    AdjList g4(3);
    g4[0].push_back({1, 0});
    g4[1].push_back({2, 0});
    ShortestPaths sp4 = dijkstra(g4, 0);
    assert(sp4.dist[1] == 0);
    assert(sp4.dist[2] == 0);
    assert(reconstruct_path(sp4, 0, 2) == std::vector<int>({0, 1, 2}));

    // negative weight throws
    {
        AdjList neg(2);
        neg[0].push_back({1, -1});
        bool threw = false;
        try {
            dijkstra(neg, 0);
        } catch (const std::invalid_argument&) {
            threw = true;
        }
        assert(threw);
    }

    std::cout << "dijkstra: all tests passed\n";

    // demo
    std::cout << "demo: shortest distances from 0 = {";
    for (int v = 0; v < 5; ++v) {
        std::cout << (v ? ", " : "") << "v" << v << ":" << sp.dist[v];
    }
    std::cout << "}\npath 0 -> 4: ";
    for (int v : reconstruct_path(sp, 0, 4)) {
        std::cout << v << ' ';
    }
    std::cout << "\nnode 2 (from source 0 in second graph): unreachable, dist = "
              << sp2.dist[2] << '\n';
    return 0;
}
