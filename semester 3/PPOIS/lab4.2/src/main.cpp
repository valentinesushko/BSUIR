#include "graph/Graph.h"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace {

using StringGraph = graph::Graph<std::string>;

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) {
        return {};
    }
    return line;
}

int readNumber(const std::string& prompt, int minimum, int maximum) {
    while (true) {
        std::string line = readLine(prompt);
        if (line.empty() && !std::cin) {
            return -1;
        }
        try {
            std::size_t consumed = 0;
            int value = std::stoi(line, &consumed);
            if (consumed == line.size() && value >= minimum && value <= maximum) {
                return value;
            }
        } catch (const std::exception&) {
        }
        std::cout << "Enter a number from " << minimum << " to " << maximum
                  << ".\n";
    }
}

void printVertices(const StringGraph& graph) {
    std::cout << "Vertices:";
    std::for_each(graph.cbegin(), graph.cend(),
                  [](const std::string& vertex) {
                      std::cout << ' ' << vertex;
                  });
    std::cout << '\n';
}

void printVerticesReverse(const StringGraph& graph) {
    std::cout << "Vertices in reverse:";
    std::for_each(graph.crbegin(), graph.crend(),
                  [](const std::string& vertex) {
                      std::cout << ' ' << vertex;
                  });
    std::cout << '\n';
}

void printEdges(const StringGraph& graph) {
    std::cout << "Edges:";
    std::for_each(graph.cEdgesBegin(), graph.cEdgesEnd(),
                  [](const StringGraph::edge_type& edge) {
                      std::cout << " (" << edge.first << ", " << edge.second
                                << ')';
                  });
    std::cout << '\n';
}

void printEdgesReverse(const StringGraph& graph) {
    std::cout << "Edges in reverse:";
    std::for_each(graph.cEdgesRBegin(), graph.cEdgesREnd(),
                  [](const StringGraph::edge_type& edge) {
                      std::cout << " (" << edge.first << ", " << edge.second
                                << ')';
                  });
    std::cout << '\n';
}

void printVertexDetails(const StringGraph& graph, const std::string& vertex) {
    std::cout << "Degree of " << vertex << ": " << graph.degree(vertex)
              << '\n';

    std::cout << "Incident edges:";
    for (auto it = graph.incidentBegin(vertex);
         it != graph.incidentEnd(vertex); ++it) {
        std::cout << " (" << it->first << ", " << it->second << ')';
    }
    std::cout << '\n';

    std::cout << "Incident edges in reverse:";
    for (auto it = graph.incidentRBegin(vertex);
         it != graph.incidentREnd(vertex); ++it) {
        std::cout << " (" << it->first << ", " << it->second << ')';
    }
    std::cout << '\n';

    std::cout << "Adjacent vertices:";
    for (auto it = graph.adjacentBegin(vertex);
         it != graph.adjacentEnd(vertex); ++it) {
        std::cout << ' ' << *it;
    }
    std::cout << '\n';

    std::cout << "Adjacent vertices in reverse:";
    for (auto it = graph.adjacentRBegin(vertex);
         it != graph.adjacentREnd(vertex); ++it) {
        std::cout << ' ' << *it;
    }
    std::cout << '\n';

    std::cout << "Adjacent vertices (one-way iterator):";
    for (auto it = graph.forwardAdjacentBegin(vertex);
         it != graph.forwardAdjacentEnd(vertex); ++it) {
        std::cout << ' ' << *it;
    }
    std::cout << '\n';
}

void sortVertices(const StringGraph& graph) {
    std::vector<std::string> vertices(graph.cbegin(), graph.cend());
    std::sort(vertices.begin(), vertices.end());
    std::cout << "Vertices sorted with std::sort:";
    for (const std::string& vertex : vertices) {
        std::cout << ' ' << vertex;
    }
    std::cout << '\n';
}

void sortEdges(const StringGraph& graph) {
    std::vector<StringGraph::edge_type> edges(graph.cEdgesBegin(),
                                              graph.cEdgesEnd());
    std::sort(edges.begin(), edges.end(),
              [](const StringGraph::edge_type& left,
                 const StringGraph::edge_type& right) {
                  if (left.first != right.first) {
                      return left.first < right.first;
                  }
                  return left.second < right.second;
              });
    std::cout << "Edges sorted with std::sort:";
    for (const StringGraph::edge_type& edge : edges) {
        std::cout << " (" << edge.first << ", " << edge.second << ')';
    }
    std::cout << '\n';
}

void compareWithCopy(const StringGraph& graph) {
    StringGraph copy(graph);
    std::cout << "Copy == original: " << std::boolalpha << (copy == graph)
              << '\n';

    copy.addVertex("copy-only");
    std::cout << "After adding a vertex to the copy:\n";
    std::cout << "  original: " << graph << '\n';
    std::cout << "  copy:     " << copy << '\n';
    std::cout << "  copy != original: " << (copy != graph) << '\n';
    std::cout << "  original < copy: " << (graph < copy) << '\n';
    std::cout << "  original <= copy: " << (graph <= copy) << '\n';
    std::cout << "  original > copy: " << (graph > copy) << '\n';
    std::cout << "  original >= copy: " << (graph >= copy) << '\n';
}

void printMenu() {
    std::cout << "\n--- Graph menu ---\n"
              << " 1. Add vertex\n"
              << " 2. Add edge\n"
              << " 3. Remove vertex\n"
              << " 4. Remove edge\n"
              << " 5. Show graph and counts\n"
              << " 6. Show vertex details\n"
              << " 7. Iterate vertices forward and reverse\n"
              << " 8. Iterate edges forward and reverse\n"
              << " 9. Sort vertices with std::sort\n"
              << "10. Sort edges with std::sort\n"
              << "11. Compare graph with its copy\n"
              << "12. Run automatic demonstration\n"
              << " 0. Exit\n";
}

void makeDemo(StringGraph& graph) {
    graph.clear();
    for (const char* vertex : {"A", "B", "C", "D", "E"}) {
        graph.addVertex(std::string(vertex));
    }
    graph.addEdge("A", "B");
    graph.addEdge("A", "C");
    graph.addEdge("B", "D");
    graph.addEdge("C", "D");
    graph.addEdge("D", "E");
}

void runDemo(const StringGraph& graph) {
    std::cout << "\nGraph: " << graph << '\n';
    std::cout << "Vertex count: " << graph.vertexCount()
              << ", edge count: " << graph.edgeCount() << '\n';
    printVertices(graph);
    printVerticesReverse(graph);
    printEdges(graph);
    printEdgesReverse(graph);
    sortVertices(graph);
    sortEdges(graph);
    std::cout << "Degree of D: " << graph.degree("D") << '\n';
    const StringGraph::edge_degree_type degree = graph.edgeDegree("A", "B");
    std::cout << "Degree of edge (A, B): (" << degree.first << ", "
              << degree.second << ")\n";
    printVertexDetails(graph, "D");
    compareWithCopy(graph);
}

}  // namespace

int main() {
    StringGraph graph;
    makeDemo(graph);

    std::cout << "Generic undirected graph container (Wirth structure).\n";
    std::cout << "Initial graph: " << graph << '\n';

    while (true) {
        printMenu();
        const int choice = readNumber("Choice: ", 0, 12);
        if (choice < 0) {
            break;
        }

        try {
            switch (choice) {
                case 0:
                    std::cout << "Goodbye.\n";
                    return 0;
                case 1: {
                    const std::string vertex = readLine("Vertex value: ");
                    graph.addVertex(vertex);
                    std::cout << "Vertex " << vertex << " added.\n";
                    break;
                }
                case 2: {
                    const std::string from = readLine("First vertex: ");
                    const std::string to = readLine("Second vertex: ");
                    graph.addEdge(from, to);
                    std::cout << "Edge (" << from << ", " << to
                              << ") added.\n";
                    break;
                }
                case 3: {
                    const std::string vertex = readLine("Vertex value: ");
                    const std::size_t removed = graph.removeVertex(vertex);
                    std::cout << "Vertex " << vertex << " removed with "
                              << removed << " incident edge(s).\n";
                    break;
                }
                case 4: {
                    const std::string from = readLine("First vertex: ");
                    const std::string to = readLine("Second vertex: ");
                    graph.removeEdge(from, to);
                    std::cout << "Edge (" << from << ", " << to
                              << ") removed.\n";
                    break;
                }
                case 5:
                    std::cout << graph << '\n';
                    std::cout << "Vertex count: " << graph.vertexCount()
                              << ", edge count: " << graph.edgeCount()
                              << ", empty: " << std::boolalpha
                              << graph.empty() << '\n';
                    break;
                case 6: {
                    const std::string vertex = readLine("Vertex value: ");
                    printVertexDetails(graph, vertex);
                    break;
                }
                case 7:
                    printVertices(graph);
                    printVerticesReverse(graph);
                    break;
                case 8:
                    printEdges(graph);
                    printEdgesReverse(graph);
                    break;
                case 9:
                    sortVertices(graph);
                    break;
                case 10:
                    sortEdges(graph);
                    break;
                case 11:
                    compareWithCopy(graph);
                    break;
                case 12:
                    runDemo(graph);
                    break;
                default:
                    break;
            }
        } catch (const graph::GraphException& error) {
            std::cout << "Graph error [" << error.errorCode()
                      << "]: " << error.what() << '\n';
        } catch (const std::exception& error) {
            std::cout << "Error: " << error.what() << '\n';
        }
    }

    return 0;
}
