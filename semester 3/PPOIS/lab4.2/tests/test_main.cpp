#include "graph/Graph.h"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {

int failures = 0;

void expect(bool condition, const std::string& message) {
    if (!condition) {
        ++failures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

template <typename Exception, typename Function>
void expectThrows(Function function, const std::string& message,
                  const std::string& errorCode) {
    try {
        function();
        expect(false, message + " (nothing was thrown)");
    } catch (const Exception& error) {
        expect(std::string(error.errorCode()) == errorCode,
               message + " (unexpected error code)");
    } catch (...) {
        expect(false, message + " (wrong exception type)");
    }
}

template <typename Iterator>
std::vector<typename std::iterator_traits<Iterator>::value_type> collect(
    Iterator begin, Iterator end) {
    std::vector<typename std::iterator_traits<Iterator>::value_type> values;
    for (; begin != end; ++begin) {
        values.push_back(*begin);
    }
    return values;
}

void testEmptyGraphAndVertices() {
    using IntGraph = graph::Graph<int>;

    IntGraph graph;
    expect(graph.empty(), "default graph is empty");
    expect(graph.vertexCount() == 0, "empty graph has no vertices");
    expect(graph.edgeCount() == 0, "empty graph has no edges");
    expect(graph.begin() == graph.end(), "empty vertex range has begin == end");
    expect(graph.cbegin() == graph.cend(),
           "empty const vertex range has begin == end");
    expect(graph.edgesBegin() == graph.edgesEnd(),
           "empty edge range has begin == end");
    expect(graph.cEdgesBegin() == graph.cEdgesEnd(),
           "empty const edge range has begin == end");
    expect(graph.rbegin() == graph.rend(),
           "empty reverse vertex range has begin == end");
    graph.clear();
    expect(graph.empty(), "clear on an empty graph is safe");

    IntGraph::iterator inserted = graph.addVertex(10);
    expect(*inserted == 10, "addVertex returns an iterator to the new vertex");
    expect(inserted == graph.begin(), "first vertex begins the iteration");

    const int value = 20;
    IntGraph::iterator second = graph.addVertex(value);
    expect(*second == 20, "addVertex copies an lvalue");
    graph.addVertex(30);
    expect(graph.vertexCount() == 3, "three vertices were added");
    expect(!graph.empty(), "non-empty graph is not empty");

    expect(graph.containsVertex(10), "containsVertex finds an existing vertex");
    expect(!graph.containsVertex(99), "containsVertex rejects a missing vertex");
    expect(graph.findVertex(20) != graph.end(),
           "findVertex returns an iterator to a vertex");
    expect(graph.findVertex(99) == graph.end(),
           "findVertex returns end for a missing vertex");

    const IntGraph& constGraph = graph;
    expect(constGraph.findVertex(10) != constGraph.end(),
           "const findVertex finds an existing vertex");
    expect(constGraph.findVertex(99) == constGraph.end(),
           "const findVertex returns end for a missing vertex");

    expectThrows<graph::DuplicateVertexException>(
        [&graph]() { graph.addVertex(10); }, "duplicate vertex throws",
        "GRAPH-DUPLICATE-VERTEX");
    expectThrows<graph::DuplicateVertexException>(
        [&graph]() { graph.addVertex(20); }, "duplicate rvalue vertex throws",
        "GRAPH-DUPLICATE-VERTEX");

    expect(collect(graph.begin(), graph.end()) ==
               std::vector<int>({10, 20, 30}),
           "vertices iterate in insertion order");
    expect(collect(graph.rbegin(), graph.rend()) ==
               std::vector<int>({30, 20, 10}),
           "vertices iterate in reverse order");
    expect(collect(graph.crbegin(), graph.crend()) ==
               std::vector<int>({30, 20, 10}),
           "const reverse iteration works");

    IntGraph::reverse_iterator reverseIt = graph.rbegin();
    expect(*reverseIt == 30, "reverse iterator dereference works");
    expect(*(reverseIt++) == 30, "reverse post-increment returns the last value");
    expect(*reverseIt == 20, "reverse post-increment advanced the iterator");
    expect(*(reverseIt--) == 20, "reverse post-decrement returns the old value");
    expect(*reverseIt == 30, "reverse post-decrement moved the iterator back");
    ++reverseIt;
    expect(*reverseIt == 20, "reverse pre-increment moves the iterator");
    --reverseIt;
    expect(*reverseIt == 30, "reverse pre-decrement moves the iterator back");

    IntGraph::iterator it = graph.begin();
    expect(*it == 10, "vertex iterator dereference works");
    expect(*it.operator->() == 10, "vertex iterator arrow operator works");
    ++it;
    expect(*it == 20, "pre-increment moves the vertex iterator");
    expect(*(it++) == 20, "post-increment returns the old position");
    expect(*it == 30, "post-increment advanced the iterator");
    --it;
    expect(*it == 20, "pre-decrement moves the vertex iterator");
    expect(*(it--) == 20, "post-decrement returns the old position");
    expect(*it == 10, "post-decrement moved the iterator back");
    --graph.end();
    expect(*--graph.end() == 30, "decrementing end yields the last vertex");

    IntGraph::const_iterator constIt = graph.begin();
    expect(constIt == graph.begin(), "mutable iterator converts to const");
    expect(graph.begin() == graph.cbegin(),
           "mutable and const iterators compare equal");
    expect(graph.cbegin() != graph.cend(),
           "cross-type iterator inequality works");

    *graph.begin() = 11;
    expect(graph.containsVertex(11), "vertex value is mutable through iterator");
    expect(!graph.containsVertex(10), "old vertex value is gone");

    IntGraph::iterator defaultIterator;
    IntGraph::const_iterator defaultConstIterator;
    expect(defaultIterator == IntGraph::iterator(),
           "default vertex iterators compare equal");
    expect(defaultConstIterator == IntGraph::const_iterator(),
           "default const vertex iterators compare equal");
}

void testEdgesAndDegrees() {
    using IntGraph = graph::Graph<int>;

    IntGraph graph({1, 2, 3, 4});
    IntGraph::EdgeIterator firstEdge = graph.addEdge(1, 2);
    expect((*firstEdge) == std::make_pair(1, 2),
           "edge iterator returns a pair of values");
    expect(firstEdge->first == 1 && firstEdge->second == 2,
           "edge iterator arrow operator exposes the pair");
    graph.addEdge(1, 3);
    graph.addEdge(2, 3);
    graph.addEdge(3, 4);

    expect(graph.edgeCount() == 4, "four edges were added");
    expect(graph.containsEdge(1, 2), "containsEdge finds a stored edge");
    expect(graph.containsEdge(2, 1),
           "containsEdge treats the graph as undirected");
    expect(!graph.containsEdge(1, 4),
           "containsEdge rejects a missing edge");
    expect(!graph.containsEdge(1, 99),
           "containsEdge returns false for a missing vertex");
    expect(!graph.containsEdge(99, 1),
           "containsEdge returns false for a missing source vertex");

    expect(graph.findEdge(2, 1) != graph.edgesEnd(),
           "findEdge finds an undirected edge");
    expect(graph.findEdge(1, 4) == graph.edgesEnd(),
           "findEdge returns end for a missing edge");
    expect(graph.findEdge(1, 99) == graph.edgesEnd(),
           "findEdge returns end for a missing vertex");

    const IntGraph& constGraph = graph;
    expect(constGraph.findEdge(1, 2) != constGraph.edgesEnd(),
           "const findEdge finds an edge");
    expect(constGraph.findEdge(1, 4) == constGraph.edgesEnd(),
           "const findEdge returns end for a missing edge");
    expect(constGraph.findEdge(99, 1) == constGraph.edgesEnd(),
           "const findEdge returns end for a missing source vertex");
    expect(constGraph.findEdge(1, 99) == constGraph.edgesEnd(),
           "const findEdge returns end for a missing target vertex");

    expect(graph.degree(1) == 2, "degree counts incident edges");
    expect(graph.degree(3) == 3, "degree of a central vertex is three");
    expect(graph.degree(4) == 1, "degree of a leaf is one");

    const IntGraph::edge_degree_type edgeDegree = graph.edgeDegree(1, 3);
    expect(edgeDegree.first == 2 && edgeDegree.second == 3,
           "edgeDegree returns the degrees of both endpoints");
    expect((graph.edgeDegree(3, 1) == IntGraph::edge_degree_type(3, 2)),
           "edgeDegree is independent of endpoint order");

    expectThrows<graph::VertexNotFoundException>(
        [&graph]() { graph.degree(99); }, "degree of a missing vertex throws",
        "GRAPH-VERTEX-NOT-FOUND");
    expectThrows<graph::VertexNotFoundException>(
        [&graph]() { graph.edgeDegree(99, 1); },
        "edgeDegree for a missing source vertex throws",
        "GRAPH-VERTEX-NOT-FOUND");
    expectThrows<graph::VertexNotFoundException>(
        [&graph]() { graph.edgeDegree(1, 99); },
        "edgeDegree for a missing target vertex throws",
        "GRAPH-VERTEX-NOT-FOUND");
    expectThrows<graph::EdgeNotFoundException>(
        [&graph]() { graph.edgeDegree(1, 4); },
        "edgeDegree for a missing edge throws", "GRAPH-EDGE-NOT-FOUND");

    expectThrows<graph::DuplicateEdgeException>(
        [&graph]() { graph.addEdge(1, 2); }, "duplicate edge throws",
        "GRAPH-DUPLICATE-EDGE");
    expectThrows<graph::DuplicateEdgeException>(
        [&graph]() { graph.addEdge(2, 1); },
        "duplicate edge in reverse order throws", "GRAPH-DUPLICATE-EDGE");
    expectThrows<graph::VertexNotFoundException>(
        [&graph]() { graph.addEdge(99, 1); },
        "edge with a missing source vertex throws",
        "GRAPH-VERTEX-NOT-FOUND");
    expectThrows<graph::VertexNotFoundException>(
        [&graph]() { graph.addEdge(1, 99); },
        "edge with a missing target vertex throws",
        "GRAPH-VERTEX-NOT-FOUND");
    expectThrows<graph::LoopNotSupportedException>(
        [&graph]() { graph.addEdge(2, 2); }, "self-loop throws",
        "GRAPH-LOOP-NOT-SUPPORTED");

    expect(graph.removeEdge(1, 2) == 1, "removeEdge reports one removed edge");
    expect(graph.edgeCount() == 3, "edge count decreased after removeEdge");
    expect(!graph.containsEdge(1, 2), "removed edge is absent");
    expectThrows<graph::EdgeNotFoundException>(
        [&graph]() { graph.removeEdge(1, 2); },
        "removing a missing edge throws", "GRAPH-EDGE-NOT-FOUND");
    expectThrows<graph::VertexNotFoundException>(
        [&graph]() { graph.removeEdge(99, 1); },
        "removing an edge with a missing vertex throws",
        "GRAPH-VERTEX-NOT-FOUND");

    graph.addEdge(1, 2);
    expect(graph.containsEdge(1, 2), "edge can be re-added after removal");
}

void testRemoveVertex() {
    using IntGraph = graph::Graph<int>;

    IntGraph graph({1, 2, 3, 4});
    graph.addEdge(1, 2);
    graph.addEdge(1, 3);
    graph.addEdge(2, 3);

    expect(graph.removeVertex(1) == 2,
           "removeVertex reports its incident edge count");
    expect(!graph.containsVertex(1), "removed vertex is absent");
    expect(graph.edgeCount() == 1,
           "removeVertex removes all incident edges");
    expect(graph.removeVertex(4) == 0, "isolated vertex removes zero edges");
    expectThrows<graph::VertexNotFoundException>(
        [&graph]() { graph.removeVertex(99); },
        "removing a missing vertex throws", "GRAPH-VERTEX-NOT-FOUND");

    expect(graph.removeVertex(2) == 1,
           "removeVertex removes the last incident edge");
    expect(graph.vertexCount() == 1, "one vertex remains");
    expect(graph.removeVertex(3) == 0, "final vertex can be removed");
    expect(graph.empty(), "graph is empty after removing all vertices");
    expect(graph.begin() == graph.end(), "empty graph iterates as empty");
}

void testVertexIteratorMutationAndErase() {
    using IntGraph = graph::Graph<int>;

    IntGraph graph({1, 2, 3, 4});
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);
    graph.addEdge(3, 4);

    for (IntGraph::iterator it = graph.begin(); it != graph.end(); ++it) {
        *it += 10;
    }
    expect(graph.containsVertex(11), "all vertices were modified");
    expect(graph.containsEdge(11, 12),
           "edge endpoints follow modified vertex values");

    IntGraph::iterator second = graph.begin();
    ++second;
    IntGraph::iterator afterErase = graph.eraseVertex(second);
    expect(*afterErase == 13, "eraseVertex returns the following iterator");
    expect(graph.vertexCount() == 3, "eraseVertex reduced the vertex count");
    expect(graph.edgeCount() == 1,
           "eraseVertex removed incident edges");

    IntGraph::iterator first = graph.eraseVertex(graph.begin());
    expect(*first == 13, "eraseVertex can remove the first vertex");
    expect(graph.vertexCount() == 2, "two vertices remain");

    IntGraph::const_iterator constPosition = graph.cbegin();
    IntGraph::iterator afterConstErase = graph.eraseVertex(constPosition);
    expect(*afterConstErase == 14,
           "eraseVertex accepts a const_iterator and returns the next vertex");
    expect(graph.vertexCount() == 1, "one vertex remains after const erase");

    expectThrows<graph::InvalidIteratorException>(
        [&graph]() { graph.eraseVertex(graph.end()); },
        "erasing end throws", "GRAPH-INVALID-ITERATOR");

    IntGraph other({50});
    expectThrows<graph::InvalidIteratorException>(
        [&graph, &other]() { graph.eraseVertex(other.begin()); },
        "erasing a foreign vertex iterator throws",
        "GRAPH-INVALID-ITERATOR");

    graph.eraseVertex(graph.begin());
    expect(graph.empty(), "eraseVertex can empty the graph");
    expect(graph.edgesBegin() == graph.edgesEnd(),
           "erasing every vertex removes every edge");
}

void testEdgeAndIncidentIterators() {
    using StringGraph = graph::Graph<std::string>;

    StringGraph graph({"A", "B", "C", "D", "E"});
    graph.addEdge("A", "B");
    graph.addEdge("A", "C");
    graph.addEdge("B", "C");
    graph.addEdge("C", "D");

    const std::vector<StringGraph::edge_type> forwardEdges =
        collect(graph.edgesBegin(), graph.edgesEnd());
    expect(forwardEdges.size() == 4, "edge iteration visits every edge");
    expect(forwardEdges.front() == std::make_pair(std::string("A"),
                                                  std::string("B")),
           "edge iteration starts at the first inserted edge");

    const std::vector<StringGraph::edge_type> reverseEdges =
        collect(graph.edgesRBegin(), graph.edgesREnd());
    expect(reverseEdges ==
               std::vector<StringGraph::edge_type>(forwardEdges.rbegin(),
                                                   forwardEdges.rend()),
           "reverse edge iteration mirrors forward iteration");

    const std::vector<StringGraph::edge_type> constEdges =
        collect(graph.cEdgesBegin(), graph.cEdgesEnd());
    expect(constEdges == forwardEdges, "const edge iteration matches");

    const std::vector<StringGraph::edge_type> constReverseEdges =
        collect(graph.cEdgesRBegin(), graph.cEdgesREnd());
    expect(constReverseEdges == reverseEdges,
           "const reverse edge iteration matches");

    expect(graph.edgesRBegin()->first == forwardEdges.back().first &&
               graph.edgesRBegin()->second == forwardEdges.back().second,
           "reverse edge iterator arrow operator works");

    StringGraph::EdgeIterator edgeIt = graph.edgesBegin();
    expect(edgeIt->first == "A", "edge arrow operator works");
    expect(*(edgeIt++) == std::make_pair(std::string("A"), std::string("B")),
           "edge post-increment returns the old edge");
    expect(*edgeIt == std::make_pair(std::string("A"), std::string("C")),
           "edge post-increment advanced the iterator");
    --edgeIt;
    expect(*edgeIt == std::make_pair(std::string("A"), std::string("B")),
           "edge pre-decrement moves back");
    ++edgeIt;
    expect(*(edgeIt--) == std::make_pair(std::string("A"), std::string("C")),
           "edge post-decrement returns the old edge");
    expect(*edgeIt == std::make_pair(std::string("A"), std::string("B")),
           "edge post-decrement moved back");
    expect(*--graph.edgesEnd() == forwardEdges.back(),
           "decrementing edge end yields the last edge");

    StringGraph::ConstEdgeIterator constEdgeIt = graph.edgesBegin();
    expect(constEdgeIt == graph.cEdgesBegin(),
           "mutable edge iterator converts to const");
    expect(graph.edgesBegin() == graph.cEdgesBegin(),
           "mutable and const edge iterators compare equal");
    expect(graph.cEdgesBegin() != graph.cEdgesEnd(),
           "cross-type edge iterator inequality works");

    const std::vector<std::string> forwardAdjacent =
        collect(graph.adjacentBegin("C"), graph.adjacentEnd("C"));
    expect(forwardAdjacent.size() == 3, "adjacent iteration visits neighbours");
    const std::vector<std::string> reverseAdjacent =
        collect(graph.adjacentRBegin("C"), graph.adjacentREnd("C"));
    expect(reverseAdjacent ==
               std::vector<std::string>(forwardAdjacent.rbegin(),
                                        forwardAdjacent.rend()),
           "reverse adjacent iteration mirrors forward iteration");

    const std::vector<std::string> forwardOnlyAdjacent = collect(
        graph.forwardAdjacentBegin("C"), graph.forwardAdjacentEnd("C"));
    expect(forwardOnlyAdjacent == forwardAdjacent,
           "one-way adjacent iterator matches bidirectional iteration");

    expect(!graph.adjacentRBegin("C")->empty(),
           "reverse adjacent iterator arrow operator works");

    StringGraph::AdjacentVertexIterator adjacentIt = graph.adjacentBegin("C");
    expect(!adjacentIt->empty(), "adjacent iterator arrow operator works");
    const std::string firstAdjacent = *adjacentIt;
    expect(*(adjacentIt++) == firstAdjacent,
           "adjacent post-increment returns the old value");
    --adjacentIt;
    expect(*adjacentIt == firstAdjacent,
           "adjacent pre-decrement returns to the first neighbour");
    --graph.adjacentEnd("C");
    expect(*--graph.adjacentEnd("C") == forwardAdjacent.back(),
           "decrementing adjacent end yields the last neighbour");

    StringGraph::ForwardAdjacentVertexIterator forwardIt =
        graph.forwardAdjacentBegin("C");
    expect(!forwardIt->empty(),
           "one-way adjacent iterator arrow operator works");
    expect(*(forwardIt++) == firstAdjacent,
           "one-way adjacent post-increment works");
    expect(*forwardIt != firstAdjacent,
           "one-way adjacent iterator advanced");

    const std::vector<StringGraph::edge_type> incidentEdges =
        collect(graph.incidentBegin("C"), graph.incidentEnd("C"));
    expect(incidentEdges.size() == 3, "incident iteration visits all edges");
    const std::vector<StringGraph::edge_type> reverseIncidentEdges =
        collect(graph.incidentRBegin("C"), graph.incidentREnd("C"));
    expect(reverseIncidentEdges ==
               std::vector<StringGraph::edge_type>(incidentEdges.rbegin(),
                                                   incidentEdges.rend()),
           "reverse incident iteration mirrors forward iteration");

    expect(graph.incidentRBegin("C")->first ==
               reverseIncidentEdges.front().first &&
               graph.incidentRBegin("C")->second ==
               reverseIncidentEdges.front().second,
           "reverse incident iterator arrow operator works");

    StringGraph::IncidentEdgeIterator incidentIt = graph.incidentBegin("C");
    expect(incidentIt->first == "A" || incidentIt->first == "B" ||
               incidentIt->first == "C",
           "incident edge arrow operator works");
    const StringGraph::edge_type firstIncident = *incidentIt;
    expect(*(incidentIt++) == firstIncident,
           "incident post-increment returns the old edge");
    --incidentIt;
    expect(*incidentIt == firstIncident,
           "incident pre-decrement returns the first edge");
    expect(*--graph.incidentEnd("C") == incidentEdges.back(),
           "decrementing incident end yields the last incident edge");

    const StringGraph::ConstIncidentEdgeIterator constIncidentIt =
        graph.incidentBegin("C");
    expect(constIncidentIt == graph.incidentBegin("C"),
           "incident iterator converts to const");
    const StringGraph::ConstAdjacentVertexIterator constAdjacentIt =
        graph.adjacentBegin("C");
    expect(constAdjacentIt == graph.adjacentBegin("C"),
           "adjacent iterator converts to const");
    const StringGraph::ConstForwardAdjacentVertexIterator constForwardIt =
        graph.forwardAdjacentBegin("C");
    expect(constForwardIt == graph.forwardAdjacentBegin("C"),
           "one-way adjacent iterator converts to const");

    const StringGraph& constGraph = graph;
    expect(collect(constGraph.adjacentBegin("C"), constGraph.adjacentEnd("C")) ==
               forwardAdjacent,
           "const adjacent iteration matches");
    expect(collect(constGraph.incidentBegin("C"), constGraph.incidentEnd("C")) ==
               incidentEdges,
           "const incident iteration matches");
    expect(collect(constGraph.forwardAdjacentBegin("C"),
                   constGraph.forwardAdjacentEnd("C")) == forwardAdjacent,
           "const one-way adjacent iteration matches");
    expect(collect(constGraph.edgesBegin(), constGraph.edgesEnd()) ==
               forwardEdges,
           "const edge range matches");
    expect(collect(constGraph.adjacentRBegin("C"),
                   constGraph.adjacentREnd("C")) == reverseAdjacent,
           "const reverse adjacent iteration matches");
    expect(collect(constGraph.incidentRBegin("C"),
                   constGraph.incidentREnd("C")) == reverseIncidentEdges,
           "const reverse incident iteration matches");
    expect(collect(constGraph.edgesRBegin(), constGraph.edgesREnd()) ==
               reverseEdges,
           "const reverse edge range matches");

    expect(graph.incidentBegin("E") == graph.incidentEnd("E"),
           "isolated vertex has an empty incident range");
    expect(graph.adjacentBegin("E") == graph.adjacentEnd("E"),
           "isolated vertex has an empty adjacent range");
    expect(graph.forwardAdjacentBegin("E") == graph.forwardAdjacentEnd("E"),
           "isolated vertex has an empty one-way adjacent range");

    const StringGraph::IncidentEdgeIterator emptyIncident =
        graph.incidentEnd("E");
    (void)emptyIncident;
    const StringGraph::AdjacentVertexIterator emptyAdjacent =
        graph.adjacentEnd("E");
    (void)emptyAdjacent;
    const StringGraph::ForwardAdjacentVertexIterator emptyForward =
        graph.forwardAdjacentEnd("E");
    (void)emptyForward;

    expectThrows<graph::VertexNotFoundException>(
        [&graph]() { graph.incidentBegin("Z"); },
        "incident iteration of a missing vertex throws",
        "GRAPH-VERTEX-NOT-FOUND");
    expectThrows<graph::VertexNotFoundException>(
        [&graph]() { graph.incidentEnd("Z"); },
        "incident end of a missing vertex throws", "GRAPH-VERTEX-NOT-FOUND");
    expectThrows<graph::VertexNotFoundException>(
        [&graph]() { graph.adjacentBegin("Z"); },
        "adjacent iteration of a missing vertex throws",
        "GRAPH-VERTEX-NOT-FOUND");
    expectThrows<graph::VertexNotFoundException>(
        [&graph]() { graph.adjacentEnd("Z"); },
        "adjacent end of a missing vertex throws", "GRAPH-VERTEX-NOT-FOUND");
    expectThrows<graph::VertexNotFoundException>(
        [&graph]() { graph.forwardAdjacentBegin("Z"); },
        "one-way adjacent iteration of a missing vertex throws",
        "GRAPH-VERTEX-NOT-FOUND");
    expectThrows<graph::VertexNotFoundException>(
        [&graph]() { graph.forwardAdjacentEnd("Z"); },
        "one-way adjacent end of a missing vertex throws",
        "GRAPH-VERTEX-NOT-FOUND");
}

void testEdgeErase() {
    using IntGraph = graph::Graph<int>;

    IntGraph graph({1, 2, 3, 4, 5});
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);
    graph.addEdge(3, 4);
    graph.addEdge(1, 4);

    IntGraph::EdgeIterator second = graph.edgesBegin();
    ++second;
    IntGraph::EdgeIterator afterErase = graph.eraseEdge(second);
    expect((*afterErase) == std::make_pair(3, 4),
           "eraseEdge returns the following edge");
    expect(graph.edgeCount() == 3, "eraseEdge reduced the edge count");

    IntGraph::ConstEdgeIterator constPosition = graph.cEdgesBegin();
    IntGraph::EdgeIterator afterConstErase = graph.eraseEdge(constPosition);
    expect((*afterConstErase) == std::make_pair(3, 4),
           "eraseEdge accepts a const_iterator");
    expect(graph.edgeCount() == 2, "const eraseEdge reduced the edge count");

    expectThrows<graph::InvalidIteratorException>(
        [&graph]() { graph.eraseEdge(graph.edgesEnd()); },
        "erasing edge end throws", "GRAPH-INVALID-ITERATOR");

    IntGraph other({9, 10});
    other.addEdge(9, 10);
    expectThrows<graph::InvalidIteratorException>(
        [&graph, &other]() { graph.eraseEdge(other.edgesBegin()); },
        "erasing a foreign edge iterator throws",
        "GRAPH-INVALID-ITERATOR");

    graph.eraseEdge(graph.edgesBegin());
    graph.eraseEdge(graph.edgesBegin());
    expect(graph.edgeCount() == 0, "all edges can be erased by iterator");
    expect(graph.vertexCount() == 5, "erasing edges keeps vertices");
}

void testCopyMoveAssignmentAndSwap() {
    using StringGraph = graph::Graph<std::string>;

    StringGraph original({"A", "B", "C"});
    original.addEdge("A", "B");
    original.addEdge("B", "C");

    StringGraph copy(original);
    expect(copy == original, "copy constructor creates an equal graph");
    copy.addVertex("D");
    expect(copy != original, "copy constructor performs a deep copy");
    expect(!original.containsVertex("D"),
           "modifying the copy does not modify the original");

    StringGraph assigned;
    assigned = original;
    expect(assigned == original, "copy assignment copies the whole graph");
    assigned = assigned;
    expect(assigned == original, "self copy assignment is safe");

    copy = original;
    StringGraph moved(std::move(copy));
    expect(moved == original, "move constructor transfers the graph");
    expect(copy.empty(), "moved-from graph is empty");
    copy.addVertex("E");
    expect(copy.containsVertex("E"), "moved-from graph is reusable");

    StringGraph moveAssigned;
    moveAssigned.addVertex("old");
    moveAssigned = std::move(moved);
    expect(moveAssigned == original, "move assignment transfers the graph");
    expect(moved.empty(), "move-assigned source becomes empty");
    moveAssigned = std::move(moveAssigned);
    expect(moveAssigned == original, "self move assignment is safe");

    StringGraph left(original);
    StringGraph right;
    left.swap(right);
    expect(left.empty(), "member swap moves the source graph out");
    expect(right == original, "member swap moves the graph in");
    swap(left, right);
    expect(left == original, "free swap restores the source graph");
    expect(right.empty(), "free swap moves the other graph out");

    left.clear();
    expect(left.empty(), "clear empties a non-empty graph");
    left.addVertex("Z");
    expect(left.vertexCount() == 1, "graph is usable after clear");

    StringGraph emptyCopy(left);
    emptyCopy.clear();
    expect(emptyCopy.empty(), "copy of a non-empty graph can be cleared");
}

void testComparisonOperators() {
    using IntGraph = graph::Graph<int>;

    IntGraph first({1, 2, 3});
    first.addEdge(1, 2);
    first.addEdge(1, 3);

    IntGraph second({3, 1, 2});
    second.addEdge(3, 1);
    second.addEdge(2, 1);

    expect(first == second,
           "graph equality ignores insertion order of vertices and edges");
    expect(!(first != second), "equal graphs are not unequal");
    expect(!(first < second), "equal graphs are not less");
    expect(!(first > second), "equal graphs are not greater");
    expect(first <= second, "equal graphs are less-or-equal");
    expect(first >= second, "equal graphs are greater-or-equal");

    IntGraph fewer({1, 2});
    expect(fewer != first, "graphs with different vertices differ");
    expect(fewer < first, "graph with fewer vertices is less");
    expect(first > fewer, "graph with more vertices is greater");
    expect(fewer <= first, "fewer vertices is less-or-equal");
    expect(first >= fewer, "more vertices is greater-or-equal");

    IntGraph largerValue({1, 4});
    expect(fewer < largerValue,
           "lexicographic comparison uses vertex values");
    expect(largerValue > fewer,
           "lexicographic comparison works in both directions");
    expect(!(largerValue < fewer), "larger vertex value is not less");

    IntGraph noEdges({1, 2, 3});
    expect(noEdges < first, "graph without edges is less");
    expect(first > noEdges, "graph with edges is greater");
    expect(noEdges <= first, "no-edge graph is less-or-equal");
    expect(first >= noEdges, "edge graph is greater-or-equal");

    IntGraph differentEdge({1, 2, 3});
    differentEdge.addEdge(2, 3);
    expect(differentEdge != first,
           "graphs with different edges are unequal");
    expect(differentEdge < first || first < differentEdge,
           "different edge sets have a strict order");

    IntGraph sameCountOtherVertex({1, 2, 4});
    expect(noEdges != sameCountOtherVertex,
           "equality rejects a different vertex with the same count");

    IntGraph sameVerticesOtherEdge({1, 2, 3});
    sameVerticesOtherEdge.addEdge(1, 2);
    IntGraph sameVerticesAnotherEdge({1, 2, 3});
    sameVerticesAnotherEdge.addEdge(1, 3);
    expect(sameVerticesOtherEdge != sameVerticesAnotherEdge,
           "equality rejects a different edge with the same count");

    IntGraph empty;
    expect(empty < fewer, "empty graph is less than any non-empty graph");
    expect(fewer > empty, "non-empty graph is greater than empty");
    expect(empty <= empty, "empty graph is less-or-equal to itself");
    expect(empty >= empty, "empty graph is greater-or-equal to itself");
}

void testStreaming() {
    using IntGraph = graph::Graph<int>;

    IntGraph empty;
    std::ostringstream emptyStream;
    emptyStream << empty;
    expect(emptyStream.str() == "Graph{vertices=[], edges=[]}",
           "empty graph has the expected textual representation");

    IntGraph graph({1, 2, 3});
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);
    std::ostringstream stream;
    stream << graph;
    expect(stream.str() == "Graph{vertices=[1, 2, 3], edges=[(1, 2), (2, 3)]}",
           "non-empty graph has the expected textual representation");

    graph::Graph<std::string> strings({"alpha", "beta"});
    strings.addEdge("alpha", "beta");
    std::ostringstream stringStream;
    stringStream << strings;
    expect(stringStream.str() ==
               "Graph{vertices=[alpha, beta], edges=[(alpha, beta)]}",
           "streaming works for string values");
}

void testInitializerListConstructors() {
    using IntGraph = graph::Graph<int>;

    IntGraph verticesOnly({1, 2, 3});
    expect(verticesOnly.vertexCount() == 3,
           "initializer-list vertex constructor adds vertices");
    expect(verticesOnly.edgeCount() == 0,
           "initializer-list vertex constructor adds no edges");

    IntGraph full({1, 2, 3}, {{1, 2}, {2, 3}});
    expect(full.vertexCount() == 3, "initializer-list graph has vertices");
    expect(full.edgeCount() == 2, "initializer-list graph has edges");
    expect(full.containsEdge(1, 2) && full.containsEdge(2, 3),
           "initializer-list edges are stored");
}

void testExceptions() {
    using IntGraph = graph::Graph<int>;

    const graph::GraphException general("general");
    expect(std::string(general.errorCode()) == "GRAPH-GENERAL",
           "base graph exception has a general error code");

    const graph::VertexNotFoundException vertex;
    expect(std::string(vertex.errorCode()) == "GRAPH-VERTEX-NOT-FOUND",
           "vertex exception error code");
    const graph::EdgeNotFoundException edge;
    expect(std::string(edge.errorCode()) == "GRAPH-EDGE-NOT-FOUND",
           "edge exception error code");
    const graph::DuplicateVertexException duplicateVertex;
    expect(std::string(duplicateVertex.errorCode()) ==
               "GRAPH-DUPLICATE-VERTEX",
           "duplicate vertex exception error code");
    const graph::DuplicateEdgeException duplicateEdge;
    expect(std::string(duplicateEdge.errorCode()) == "GRAPH-DUPLICATE-EDGE",
           "duplicate edge exception error code");
    const graph::LoopNotSupportedException loop;
    expect(std::string(loop.errorCode()) == "GRAPH-LOOP-NOT-SUPPORTED",
           "loop exception error code");
    const graph::InvalidIteratorException iterator;
    expect(std::string(iterator.errorCode()) == "GRAPH-INVALID-ITERATOR",
           "invalid iterator exception error code");

    IntGraph graph;
    graph.addVertex(1);
    bool caughtByBase = false;
    try {
        graph.addVertex(1);
    } catch (const graph::GraphException& error) {
        caughtByBase =
            std::string(error.errorCode()) == "GRAPH-DUPLICATE-VERTEX";
    }
    expect(caughtByBase, "graph exceptions can be caught by the base class");
}

void testEdgeRemovalBranches() {
    using IntGraph = graph::Graph<int>;

    IntGraph graph({1, 2, 3, 4});
    graph.addEdge(1, 2);
    graph.addEdge(1, 3);
    graph.addEdge(1, 4);
    graph.addEdge(2, 3);

    graph.removeEdge(1, 4);
    expect(!graph.containsEdge(1, 4), "head incident edge can be removed");
    graph.removeEdge(1, 2);
    expect(!graph.containsEdge(1, 2), "middle incident edge can be removed");
    graph.removeEdge(1, 3);
    expect(graph.degree(1) == 0, "tail incident edge can be removed");

    graph.removeVertex(2);
    expect(!graph.containsVertex(2), "vertex with one edge can be removed");
    graph.removeVertex(3);
    expect(!graph.containsVertex(3), "vertex with one edge can be removed");
    graph.removeVertex(4);
    expect(graph.vertexCount() == 1,
           "removing isolated vertices keeps the remaining vertex");
    expect(graph.containsVertex(1),
           "the remaining vertex is still present");
    graph.removeVertex(1);
    expect(graph.empty(), "removing the last vertex empties the graph");
}

}  // namespace

int main() {
    testEmptyGraphAndVertices();
    testEdgesAndDegrees();
    testRemoveVertex();
    testVertexIteratorMutationAndErase();
    testEdgeAndIncidentIterators();
    testEdgeErase();
    testCopyMoveAssignmentAndSwap();
    testComparisonOperators();
    testStreaming();
    testInitializerListConstructors();
    testExceptions();
    testEdgeRemovalBranches();

    if (failures != 0) {
        std::cerr << failures << " test(s) failed\n";
        return 1;
    }

    std::cout << "All graph tests passed\n";
    return 0;
}
