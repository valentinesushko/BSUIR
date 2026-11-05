# Generic Undirected Graph Container

A C++17 implementation of the generic graph container required by the
laboratory variant. The graph is undirected and uses Wirth's linked structure:
vertices are stored in a doubly linked list, and every vertex owns a doubly
linked list of incident edges. Each edge node belongs to the incident lists of
both endpoints and to the global edge list. The internal representation is
private; users work only with the public methods and iterators.

The container has a single template parameter `T` that defines the type of the
value stored in a vertex. The implementation is header-only.

## Requirements

- CMake 3.16 or newer
- A C++17 compatible compiler
- No third-party libraries are required

## Build

From this directory:

```sh
cmake -S . -B build
cmake --build build
```

This builds:

- `graph_app` — the interactive console menu and demonstration
- `graph_tests` — the self-contained test executable

## Run the console menu

```sh
./build/graph_app
```

The menu demonstrates:

- adding and removing vertices and edges;
- vertex and edge counts, emptiness, degree of a vertex and degree of an edge;
- forward and reverse iteration over vertices and edges;
- incident edge iteration and adjacent vertex iteration, including the
  additional one-way adjacent iterator;
- sorting vertices and edges with the generic `std::sort` algorithm applied to
  data collected through the container iterators;
- copying a graph and comparing graphs with every relational operator;
- textual output through `operator<<`;
- exception handling for invalid operations.

## Run the tests

```sh
ctest --test-dir build --output-on-failure
```

The tests cover construction, copying, moving, assignment, all iterators and
their const/reverse variants, erasing by iterator, comparison operators,
streaming, every exception path, and the console-facing API. Measured line
coverage of `Graph.h` is above 98% (branch coverage is also reported by
`gcov`).

### Coverage report

```sh
cmake -S . -B build-cov -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build-cov
./build-cov/graph_tests
cd build-cov/CMakeFiles/graph_tests.dir/tests
gcov -b -c test_main.cpp.gcda
```

The report creates `Graph.h.gcov` and `Exceptions.h.gcov` with per-line
coverage information.

## Public types

The class provides the usual container aliases:

```cpp
template <typename T>
class graph::Graph {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using edge_type = std::pair<value_type, value_type>;
    using edge_degree_type = std::pair<size_type, size_type>;
    // ...
};
```

An edge is represented as `std::pair<value_type, value_type>`. The pair uses
the endpoint order in which the edge was added; membership checks and removal
accept both endpoint orders.

## Basic operations

```cpp
graph::Graph<std::string> g;

g.addVertex("A");
g.addVertex("B");
g.addEdge("A", "B");

g.containsVertex("A");        // true
g.containsEdge("B", "A");     // true
g.vertexCount();              // 2
g.edgeCount();                // 1
g.degree("A");                // 1
g.edgeDegree("A", "B");       // (1, 1)

g.removeEdge("A", "B");
g.removeVertex("A");
g.empty();                    // false
g.clear();                    // empty again
```

The class also provides constructors from initializer lists:

```cpp
graph::Graph<int> g({1, 2, 3}, {{1, 2}, {2, 3}});
```

`edgeDegree(from, to)` returns the pair of endpoint degrees, which is the
standard graph-theoretic characteristic of an edge in exercise literature:
`(deg(from), deg(to))`.

## Iterators

Every iterator is a user-defined bidirectional iterator (the additional
adjacent-vertex iterator is forward-only, as required by the variant). All
bidirectional iterators have mutable and const versions, and every mutable
container operation has const counterparts.

| Iteration | Mutable | Const | Reverse |
|---|---|---|---|
| Vertices | `begin()/end()` | `begin()/end()`, `cbegin()/cend()` | `rbegin()/rend()`, `crbegin()/crend()` |
| All edges | `edgesBegin()/edgesEnd()` | `edgesBegin()/edgesEnd()`, `cEdgesBegin()/cEdgesEnd()` | `edgesRBegin()/edgesREnd()`, `cEdgesRBegin()/cEdgesREnd()` |
| Incident edges | `incidentBegin(v)/incidentEnd(v)` | const overloads | `incidentRBegin(v)/incidentREnd(v)` |
| Adjacent vertices | `adjacentBegin(v)/adjacentEnd(v)` | const overloads | `adjacentRBegin(v)/adjacentREnd(v)` |
| Adjacent vertices, one-way | `forwardAdjacentBegin(v)/forwardAdjacentEnd(v)` | const overloads | not applicable |

Vertex iterators dereference to `T&` or `const T&`. Edge iterators dereference
to `std::pair<T, T>`. Adjacent-vertex iterators dereference to the value of the
neighbouring vertex.

Erasing by iterator:

```cpp
auto nextVertex = g.eraseVertex(g.findVertex("A"));
auto nextEdge = g.eraseEdge(g.findEdge("A", "B"));
```

`eraseVertex` removes the vertex and all incident edges. `eraseEdge` removes a
single edge. Both operations return an iterator to the following element.

## Comparison

Graphs compare structurally, independently of insertion order:

- `operator==` checks equal vertex sets and equal edge sets;
- `operator<` uses lexicographic ordering of the sorted vertex values first,
  then lexicographic ordering of the normalized endpoint-index pairs of the
  sorted edges;
- `!=`, `>`, `<=`, and `>=` are derived from those two operations.

Ordering requires `operator<` for `T`; equality requires `operator==`.

## Textual representation

`operator<<` builds the representation with `std::for_each` and the container
iterators:

```cpp
graph::Graph<int> g({1, 2, 3});
g.addEdge(1, 2);
std::cout << g;
// Graph{vertices=[1, 2, 3], edges=[(1, 2)]}
```

## Exceptions

All exceptions derive from `graph::GraphException`, which derives from
`std::runtime_error`. Every exception exposes an `errorCode()` string.

| Exception | Error code | Raised when |
|---|---|---|
| `GraphException` | `GRAPH-GENERAL` | Base class |
| `VertexNotFoundException` | `GRAPH-VERTEX-NOT-FOUND` | Vertex is absent from the graph |
| `EdgeNotFoundException` | `GRAPH-EDGE-NOT-FOUND` | Edge is absent from the graph |
| `DuplicateVertexException` | `GRAPH-DUPLICATE-VERTEX` | Adding a vertex value that already exists |
| `DuplicateEdgeException` | `GRAPH-DUPLICATE-EDGE` | Adding an edge that already exists |
| `LoopNotSupportedException` | `GRAPH-LOOP-NOT-SUPPORTED` | Adding a self-loop |
| `InvalidIteratorException` | `GRAPH-INVALID-ITERATOR` | Erasing `end()` or an iterator from another graph |

The graph is simple: self-loops and parallel edges are rejected.

## Complexity

| Operation | Time |
|---|---|
| Add vertex | `O(V)` because values are unique |
| Add edge | `O(V + deg(from))` |
| Remove vertex | `O(V + E)` in the worst case |
| Contains vertex | `O(V)` |
| Contains edge | `O(V + deg(from))` |
| Degree | `O(V + deg(vertex))` |
| Iterator increment/decrement | `O(1)` |

Memory usage is `O(V + E)` plus the stored vertex values.

## Project layout

- `include/graph/Graph.h` — the generic graph template and all iterators
- `include/graph/Exceptions.h` — the exception hierarchy
- `src/main.cpp` — the interactive console menu and demonstration
- `tests/test_main.cpp` — comprehensive unit tests
- `task.txt` — laboratory task statement
