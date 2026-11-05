#ifndef GRAPH_EXCEPTIONS_H
#define GRAPH_EXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace graph {

class GraphException : public std::runtime_error {
public:
    explicit GraphException(const std::string& message)
        : std::runtime_error(message) {}

    virtual const char* errorCode() const noexcept {
        return "GRAPH-GENERAL";
    }
};

class VertexNotFoundException : public GraphException {
public:
    explicit VertexNotFoundException(
        const std::string& message = "Graph vertex was not found")
        : GraphException(message) {}

    const char* errorCode() const noexcept override {
        return "GRAPH-VERTEX-NOT-FOUND";
    }
};

class EdgeNotFoundException : public GraphException {
public:
    explicit EdgeNotFoundException(
        const std::string& message = "Graph edge was not found")
        : GraphException(message) {}

    const char* errorCode() const noexcept override {
        return "GRAPH-EDGE-NOT-FOUND";
    }
};

class DuplicateVertexException : public GraphException {
public:
    explicit DuplicateVertexException(
        const std::string& message = "Graph vertex already exists")
        : GraphException(message) {}

    const char* errorCode() const noexcept override {
        return "GRAPH-DUPLICATE-VERTEX";
    }
};

class DuplicateEdgeException : public GraphException {
public:
    explicit DuplicateEdgeException(
        const std::string& message = "Graph edge already exists")
        : GraphException(message) {}

    const char* errorCode() const noexcept override {
        return "GRAPH-DUPLICATE-EDGE";
    }
};

class LoopNotSupportedException : public GraphException {
public:
    explicit LoopNotSupportedException(
        const std::string& message = "Self-loops are not supported")
        : GraphException(message) {}

    const char* errorCode() const noexcept override {
        return "GRAPH-LOOP-NOT-SUPPORTED";
    }
};

class InvalidIteratorException : public GraphException {
public:
    explicit InvalidIteratorException(
        const std::string& message = "Iterator does not belong to this graph")
        : GraphException(message) {}

    const char* errorCode() const noexcept override {
        return "GRAPH-INVALID-ITERATOR";
    }
};

}  // namespace graph

#endif  // GRAPH_EXCEPTIONS_H
