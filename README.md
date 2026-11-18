# Algorithms in C++

Algorithms and data structures in C++17, written from scratch. Each folder holds one program, tests run in `main()`.

## Contents

- [binary_search.cpp](01-binary-search/binary_search.cpp) — binary search, lower/upper bound
- [selection_sort.cpp](02-selection-sort/selection_sort.cpp) — selection sort
- [recursion.cpp](03-recursion/recursion.cpp) — factorial, memoized fibonacci
- [quicksort.cpp](04-quicksort/quicksort.cpp) — quicksort, median-of-three pivot
- [hash_table.cpp](05-hash-table/hash_table.cpp) — hash table, separate chaining
- [bfs.cpp](06-breadth-first-search/bfs.cpp) — breadth-first search, shortest path
- [binary_search_tree.cpp](07-binary-search-tree/binary_search_tree.cpp) — bst, insert/search/delete
- [depth_first_search.cpp](08-depth-first-search/depth_first_search.cpp) — dfs, tree traversals, cycle check
- [huffman_coding.cpp](09-huffman-coding/huffman_coding.cpp) — huffman coding
- [avl_tree.cpp](10-avl-tree/avl_tree.cpp) — avl tree, rotations
- [splay_tree.cpp](11-splay-tree/splay_tree.cpp) — splay tree
- [b_tree.cpp](12-b-tree/b_tree.cpp) — b-tree

## Run

```bash
g++ -std=c++17 -O2 -o run 01-binary-search/binary_search.cpp && ./run
```

Any other file works the same way. To run all of them:

```bash
for f in */*.cpp; do
  g++ -std=c++17 -O2 -o /tmp/alg "$f" && /tmp/alg > /dev/null \
    && echo "PASS $f" || echo "FAIL $f"
done
```
