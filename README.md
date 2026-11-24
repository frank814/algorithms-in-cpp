# Algorithms in C++

Algorithms and data structures implemented from scratch in C++17. No libraries, no framework — written to understand how each one actually works.

Any C++17 compiler works (g++, clang++, or MSVC). Every program runs its own tests in `main()` with assertions and prints a pass line when everything holds.

## What's inside

- [binary_search.cpp](binary-search/binary_search.cpp) — finds a target in a sorted list by halving the range on every step, plus lower/upper bound helpers
- [selection_sort.cpp](selection-sort/selection_sort.cpp) — repeatedly picks the smallest remaining value and swaps it into place
- [recursion.cpp](recursion/recursion.cpp) — factorial and fibonacci done recursively, with a memo table so each value is computed once
- [quicksort.cpp](quicksort/quicksort.cpp) — sorts in place by partitioning around a median-of-three pivot
- [hash_table.cpp](hash-table/hash_table.cpp) — key/value store with separate chaining, FNV-1a hashing, and automatic rehash
- [bfs.cpp](breadth-first-search/bfs.cpp) — walks a graph level by layer to find the shortest path between two nodes
- [binary_search_tree.cpp](binary-search-tree/binary_search_tree.cpp) — insert, search, and delete following one branch at a time
- [depth_first_search.cpp](depth-first-search/depth_first_search.cpp) — dives deep down one branch first: graph DFS, tree traversals, cycle detection
- [huffman_coding.cpp](huffman-coding/huffman_coding.cpp) — builds an optimal prefix code from character frequencies, encodes and decodes text
- [avl_tree.cpp](avl-tree/avl_tree.cpp) — self-balancing search tree that rotates nodes back into shape after every change
- [splay_tree.cpp](splay-tree/splay_tree.cpp) — moves the node you just touched up to the root, so hot keys rise to the top
- [b_tree.cpp](b-tree/b_tree.cpp) — each node holds several sorted keys and splits when full, the way databases store data on disk

- [dijkstra.cpp](dijkstra/dijkstra.cpp) — finds the cheapest path from one node to every other when edges have different costs

- [greedy_scheduling.cpp](greedy-scheduling/greedy_scheduling.cpp) — picks the largest set of activities that never overlap, finishing soonest first

- [greedy_knapsack.cpp](greedy-knapsack/greedy_knapsack.cpp) — fills a knapsack with the best value per unit weight first, splitting the last item if needed

## Run

```bash
g++ -std=c++17 -O2 -o run binary-search/binary_search.cpp && ./run
```

Any other file works the same way. To run all of them:

```bash
for f in */*.cpp; do
  g++ -std=c++17 -O2 -o /tmp/alg "$f" && /tmp/alg > /dev/null \
    && echo "PASS $f" || echo "FAIL $f"
done
```
