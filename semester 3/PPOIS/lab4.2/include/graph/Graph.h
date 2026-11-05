#ifndef GRAPH_GRAPH_H
#define GRAPH_GRAPH_H

#include "graph/Exceptions.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <optional>
#include <ostream>
#include <type_traits>
#include <utility>
#include <vector>

namespace graph {

template <typename T>
class Graph {
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

private:
    struct EdgeNode;

    struct VertexNode {
        explicit VertexNode(const value_type& source)
            : value(source), next(nullptr), prev(nullptr), edgesHead(nullptr) {}

        explicit VertexNode(value_type&& source)
            : value(std::move(source)),
              next(nullptr),
              prev(nullptr),
              edgesHead(nullptr) {}

        value_type value;
        VertexNode* next;
        VertexNode* prev;
        EdgeNode* edgesHead;
    };

    struct EdgeNode {
        EdgeNode(VertexNode* source, VertexNode* target)
            : from(source),
              to(target),
              nextFrom(nullptr),
              prevFrom(nullptr),
              nextTo(nullptr),
              prevTo(nullptr),
              nextGlobal(nullptr),
              prevGlobal(nullptr) {}

        VertexNode* from;
        VertexNode* to;
        EdgeNode* nextFrom;
        EdgeNode* prevFrom;
        EdgeNode* nextTo;
        EdgeNode* prevTo;
        EdgeNode* nextGlobal;
        EdgeNode* prevGlobal;
    };

    static EdgeNode*& nextSlot(EdgeNode* edge, const VertexNode* vertex) noexcept {
        return edge->from == vertex ? edge->nextFrom : edge->nextTo;
    }

    static EdgeNode*& prevSlot(EdgeNode* edge, const VertexNode* vertex) noexcept {
        return edge->from == vertex ? edge->prevFrom : edge->prevTo;
    }

    static EdgeNode* nextIncident(const EdgeNode* edge,
                                  const VertexNode* vertex) noexcept {
        return edge->from == vertex ? edge->nextFrom : edge->nextTo;
    }

    static EdgeNode* prevIncident(const EdgeNode* edge,
                                  const VertexNode* vertex) noexcept {
        return edge->from == vertex ? edge->prevFrom : edge->prevTo;
    }

    static EdgeNode* lastIncident(const VertexNode* vertex) noexcept {
        EdgeNode* current = vertex->edgesHead;
        if (current == nullptr) {
            return nullptr;
        }
        while (nextIncident(current, vertex) != nullptr) {
            current = nextIncident(current, vertex);
        }
        return current;
    }

    static void linkIncidentFront(VertexNode* vertex, EdgeNode* edge) noexcept {
        EdgeNode*& head = vertex->edgesHead;
        EdgeNode*& next = nextSlot(edge, vertex);
        EdgeNode*& prev = prevSlot(edge, vertex);

        next = head;
        prev = nullptr;
        if (head != nullptr) {
            prevSlot(head, vertex) = edge;
        }
        head = edge;
    }

    static void unlinkIncident(VertexNode* vertex, EdgeNode* edge) noexcept {
        EdgeNode*& head = vertex->edgesHead;
        EdgeNode*& next = nextSlot(edge, vertex);
        EdgeNode*& prev = prevSlot(edge, vertex);

        if (prev != nullptr) {
            nextSlot(prev, vertex) = next;
        } else {
            head = next;
        }
        if (next != nullptr) {
            prevSlot(next, vertex) = prev;
        }

        next = nullptr;
        prev = nullptr;
    }

    static size_type degreeOfNode(const VertexNode* vertex) noexcept {
        size_type result = 0;
        for (const EdgeNode* edge = vertex->edgesHead; edge != nullptr;
             edge = nextIncident(edge, vertex)) {
            ++result;
        }
        return result;
    }

public:
    template <bool IsConst>
    class BasicVertexIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using reference = typename std::conditional<IsConst, const value_type&,
                                                    value_type&>::type;
        using pointer = typename std::conditional<IsConst, const value_type*,
                                                  value_type*>::type;
        using graph_type =
            typename std::conditional<IsConst, const Graph, Graph>::type;

        BasicVertexIterator() noexcept : graph_(nullptr), node_(nullptr) {}

        template <bool B = IsConst,
                  typename std::enable_if<B, int>::type = 0>
        BasicVertexIterator(const BasicVertexIterator<false>& other) noexcept
            : graph_(other.graph_), node_(other.node_) {}

        reference operator*() const { return node_->value; }
        pointer operator->() const { return &node_->value; }

        BasicVertexIterator& operator++() {
            node_ = node_->next;
            return *this;
        }

        BasicVertexIterator operator++(int) {
            BasicVertexIterator copy(*this);
            ++(*this);
            return copy;
        }

        BasicVertexIterator& operator--() {
            node_ = node_ != nullptr ? node_->prev : graph_->verticesTail_;
            return *this;
        }

        BasicVertexIterator operator--(int) {
            BasicVertexIterator copy(*this);
            --(*this);
            return copy;
        }

        template <bool Other>
        bool operator==(const BasicVertexIterator<Other>& other) const noexcept {
            return graph_ == other.graph_ && node_ == other.node_;
        }

        template <bool Other>
        bool operator!=(const BasicVertexIterator<Other>& other) const noexcept {
            return !(*this == other);
        }

    private:
        friend class Graph;
        template <bool>
        friend class BasicVertexIterator;

        BasicVertexIterator(graph_type* graph, VertexNode* node) noexcept
            : graph_(graph), node_(node) {}

        graph_type* graph_;
        VertexNode* node_;
    };

    template <bool IsConst>
    class BasicEdgeIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = typename Graph::edge_type;
        using difference_type = std::ptrdiff_t;
        using reference = value_type;
        using pointer = const value_type*;
        using graph_type =
            typename std::conditional<IsConst, const Graph, Graph>::type;

        BasicEdgeIterator() noexcept : graph_(nullptr), node_(nullptr) {}

        template <bool B = IsConst,
                  typename std::enable_if<B, int>::type = 0>
        BasicEdgeIterator(const BasicEdgeIterator<false>& other) noexcept
            : graph_(other.graph_), node_(other.node_) {}

        value_type operator*() const {
            return value_type(node_->from->value, node_->to->value);
        }

        pointer operator->() const {
            cache_.emplace(node_->from->value, node_->to->value);
            return &cache_.value();
        }

        BasicEdgeIterator& operator++() {
            node_ = node_->nextGlobal;
            return *this;
        }

        BasicEdgeIterator operator++(int) {
            BasicEdgeIterator copy(*this);
            ++(*this);
            return copy;
        }

        BasicEdgeIterator& operator--() {
            node_ = node_ != nullptr ? node_->prevGlobal : graph_->edgesTail_;
            return *this;
        }

        BasicEdgeIterator operator--(int) {
            BasicEdgeIterator copy(*this);
            --(*this);
            return copy;
        }

        template <bool Other>
        bool operator==(const BasicEdgeIterator<Other>& other) const noexcept {
            return graph_ == other.graph_ && node_ == other.node_;
        }

        template <bool Other>
        bool operator!=(const BasicEdgeIterator<Other>& other) const noexcept {
            return !(*this == other);
        }

    private:
        friend class Graph;
        template <bool>
        friend class BasicEdgeIterator;

        BasicEdgeIterator(graph_type* graph, EdgeNode* node) noexcept
            : graph_(graph), node_(node) {}

        graph_type* graph_;
        EdgeNode* node_;
        mutable std::optional<value_type> cache_;
    };

    template <bool IsConst>
    class BasicIncidentEdgeIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = typename Graph::edge_type;
        using difference_type = std::ptrdiff_t;
        using reference = value_type;
        using pointer = const value_type*;
        using graph_type =
            typename std::conditional<IsConst, const Graph, Graph>::type;

        BasicIncidentEdgeIterator() noexcept
            : graph_(nullptr), vertex_(nullptr), node_(nullptr) {}

        template <bool B = IsConst,
                  typename std::enable_if<B, int>::type = 0>
        BasicIncidentEdgeIterator(
            const BasicIncidentEdgeIterator<false>& other) noexcept
            : graph_(other.graph_), vertex_(other.vertex_), node_(other.node_) {}

        value_type operator*() const {
            return value_type(node_->from->value, node_->to->value);
        }

        pointer operator->() const {
            cache_.emplace(node_->from->value, node_->to->value);
            return &cache_.value();
        }

        BasicIncidentEdgeIterator& operator++() {
            node_ = nextIncident(node_, vertex_);
            return *this;
        }

        BasicIncidentEdgeIterator operator++(int) {
            BasicIncidentEdgeIterator copy(*this);
            ++(*this);
            return copy;
        }

        BasicIncidentEdgeIterator& operator--() {
            node_ = node_ != nullptr ? prevIncident(node_, vertex_)
                                     : lastIncident(vertex_);
            return *this;
        }

        BasicIncidentEdgeIterator operator--(int) {
            BasicIncidentEdgeIterator copy(*this);
            --(*this);
            return copy;
        }

        template <bool Other>
        bool operator==(
            const BasicIncidentEdgeIterator<Other>& other) const noexcept {
            return graph_ == other.graph_ && vertex_ == other.vertex_ &&
                   node_ == other.node_;
        }

        template <bool Other>
        bool operator!=(
            const BasicIncidentEdgeIterator<Other>& other) const noexcept {
            return !(*this == other);
        }

    private:
        friend class Graph;
        template <bool>
        friend class BasicIncidentEdgeIterator;

        BasicIncidentEdgeIterator(graph_type* graph, VertexNode* vertex,
                                  EdgeNode* node) noexcept
            : graph_(graph), vertex_(vertex), node_(node) {}

        graph_type* graph_;
        VertexNode* vertex_;
        EdgeNode* node_;
        mutable std::optional<value_type> cache_;
    };

    template <bool IsConst>
    class BasicAdjacentVertexIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using reference = typename std::conditional<IsConst, const value_type&,
                                                    value_type&>::type;
        using pointer = typename std::conditional<IsConst, const value_type*,
                                                  value_type*>::type;
        using graph_type =
            typename std::conditional<IsConst, const Graph, Graph>::type;

        BasicAdjacentVertexIterator() noexcept
            : graph_(nullptr), vertex_(nullptr), node_(nullptr) {}

        template <bool B = IsConst,
                  typename std::enable_if<B, int>::type = 0>
        BasicAdjacentVertexIterator(
            const BasicAdjacentVertexIterator<false>& other) noexcept
            : graph_(other.graph_), vertex_(other.vertex_), node_(other.node_) {}

        reference operator*() const {
            VertexNode* other =
                node_->from == vertex_ ? node_->to : node_->from;
            return other->value;
        }

        pointer operator->() const {
            VertexNode* other =
                node_->from == vertex_ ? node_->to : node_->from;
            return &other->value;
        }

        BasicAdjacentVertexIterator& operator++() {
            node_ = nextIncident(node_, vertex_);
            return *this;
        }

        BasicAdjacentVertexIterator operator++(int) {
            BasicAdjacentVertexIterator copy(*this);
            ++(*this);
            return copy;
        }

        BasicAdjacentVertexIterator& operator--() {
            node_ = node_ != nullptr ? prevIncident(node_, vertex_)
                                     : lastIncident(vertex_);
            return *this;
        }

        BasicAdjacentVertexIterator operator--(int) {
            BasicAdjacentVertexIterator copy(*this);
            --(*this);
            return copy;
        }

        template <bool Other>
        bool operator==(
            const BasicAdjacentVertexIterator<Other>& other) const noexcept {
            return graph_ == other.graph_ && vertex_ == other.vertex_ &&
                   node_ == other.node_;
        }

        template <bool Other>
        bool operator!=(
            const BasicAdjacentVertexIterator<Other>& other) const noexcept {
            return !(*this == other);
        }

    private:
        friend class Graph;
        template <bool>
        friend class BasicAdjacentVertexIterator;

        BasicAdjacentVertexIterator(graph_type* graph, VertexNode* vertex,
                                    EdgeNode* node) noexcept
            : graph_(graph), vertex_(vertex), node_(node) {}

        graph_type* graph_;
        VertexNode* vertex_;
        EdgeNode* node_;
    };

    template <bool IsConst>
    class BasicForwardAdjacentVertexIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using reference = typename std::conditional<IsConst, const value_type&,
                                                    value_type&>::type;
        using pointer = typename std::conditional<IsConst, const value_type*,
                                                  value_type*>::type;
        using graph_type =
            typename std::conditional<IsConst, const Graph, Graph>::type;

        BasicForwardAdjacentVertexIterator() noexcept
            : graph_(nullptr), vertex_(nullptr), node_(nullptr) {}

        template <bool B = IsConst,
                  typename std::enable_if<B, int>::type = 0>
        BasicForwardAdjacentVertexIterator(
            const BasicForwardAdjacentVertexIterator<false>& other) noexcept
            : graph_(other.graph_), vertex_(other.vertex_), node_(other.node_) {}

        reference operator*() const {
            VertexNode* other =
                node_->from == vertex_ ? node_->to : node_->from;
            return other->value;
        }

        pointer operator->() const {
            VertexNode* other =
                node_->from == vertex_ ? node_->to : node_->from;
            return &other->value;
        }

        BasicForwardAdjacentVertexIterator& operator++() {
            node_ = nextIncident(node_, vertex_);
            return *this;
        }

        BasicForwardAdjacentVertexIterator operator++(int) {
            BasicForwardAdjacentVertexIterator copy(*this);
            ++(*this);
            return copy;
        }

        template <bool Other>
        bool operator==(const BasicForwardAdjacentVertexIterator<Other>& other)
            const noexcept {
            return graph_ == other.graph_ && vertex_ == other.vertex_ &&
                   node_ == other.node_;
        }

        template <bool Other>
        bool operator!=(const BasicForwardAdjacentVertexIterator<Other>& other)
            const noexcept {
            return !(*this == other);
        }

    private:
        friend class Graph;
        template <bool>
        friend class BasicForwardAdjacentVertexIterator;

        BasicForwardAdjacentVertexIterator(graph_type* graph, VertexNode* vertex,
                                           EdgeNode* node) noexcept
            : graph_(graph), vertex_(vertex), node_(node) {}

        graph_type* graph_;
        VertexNode* vertex_;
        EdgeNode* node_;
    };

    template <typename Iterator>
    class BasicReverseIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using iterator_type = Iterator;
        using value_type = typename std::iterator_traits<Iterator>::value_type;
        using difference_type =
            typename std::iterator_traits<Iterator>::difference_type;
        using reference = typename std::iterator_traits<Iterator>::reference;
        using pointer = typename std::iterator_traits<Iterator>::pointer;

        BasicReverseIterator() = default;
        explicit BasicReverseIterator(Iterator iterator) : current_(iterator) {}

        Iterator base() const { return current_; }

        reference operator*() const {
            Iterator copy = current_;
            --copy;
            return *copy;
        }

        template <bool IsReference = std::is_reference<reference>::value>
        typename std::enable_if<IsReference, pointer>::type operator->() const {
            return std::addressof(**this);
        }

        template <bool IsReference = std::is_reference<reference>::value>
        typename std::enable_if<!IsReference, pointer>::type operator->() const {
            cache_.emplace(**this);
            return std::addressof(cache_.value());
        }

        BasicReverseIterator& operator++() {
            --current_;
            return *this;
        }

        BasicReverseIterator operator++(int) {
            BasicReverseIterator copy(*this);
            ++(*this);
            return copy;
        }

        BasicReverseIterator& operator--() {
            ++current_;
            return *this;
        }

        BasicReverseIterator operator--(int) {
            BasicReverseIterator copy(*this);
            --(*this);
            return copy;
        }

        template <typename OtherIterator>
        bool operator==(
            const BasicReverseIterator<OtherIterator>& other) const {
            return current_ == other.current_;
        }

        template <typename OtherIterator>
        bool operator!=(
            const BasicReverseIterator<OtherIterator>& other) const {
            return !(*this == other);
        }

    private:
        template <typename>
        friend class BasicReverseIterator;

        Iterator current_;
        mutable std::optional<value_type> cache_;
    };

    using iterator = BasicVertexIterator<false>;
    using const_iterator = BasicVertexIterator<true>;
    using VertexIterator = iterator;
    using ConstVertexIterator = const_iterator;
    using reverse_iterator = BasicReverseIterator<iterator>;
    using const_reverse_iterator = BasicReverseIterator<const_iterator>;

    using EdgeIterator = BasicEdgeIterator<false>;
    using ConstEdgeIterator = BasicEdgeIterator<true>;
    using edge_iterator = EdgeIterator;
    using const_edge_iterator = ConstEdgeIterator;
    using reverse_edge_iterator = BasicReverseIterator<EdgeIterator>;
    using const_reverse_edge_iterator =
        BasicReverseIterator<ConstEdgeIterator>;

    using IncidentEdgeIterator = BasicIncidentEdgeIterator<false>;
    using ConstIncidentEdgeIterator = BasicIncidentEdgeIterator<true>;
    using incident_edge_iterator = IncidentEdgeIterator;
    using const_incident_edge_iterator = ConstIncidentEdgeIterator;
    using reverse_incident_edge_iterator =
        BasicReverseIterator<IncidentEdgeIterator>;
    using const_reverse_incident_edge_iterator =
        BasicReverseIterator<ConstIncidentEdgeIterator>;

    using AdjacentVertexIterator = BasicAdjacentVertexIterator<false>;
    using ConstAdjacentVertexIterator = BasicAdjacentVertexIterator<true>;
    using adjacent_vertex_iterator = AdjacentVertexIterator;
    using const_adjacent_vertex_iterator = ConstAdjacentVertexIterator;
    using reverse_adjacent_vertex_iterator =
        BasicReverseIterator<AdjacentVertexIterator>;
    using const_reverse_adjacent_vertex_iterator =
        BasicReverseIterator<ConstAdjacentVertexIterator>;

    using ForwardAdjacentVertexIterator =
        BasicForwardAdjacentVertexIterator<false>;
    using ConstForwardAdjacentVertexIterator =
        BasicForwardAdjacentVertexIterator<true>;

    Graph() noexcept
        : verticesHead_(nullptr),
          verticesTail_(nullptr),
          edgesHead_(nullptr),
          edgesTail_(nullptr),
          vertexCount_(0),
          edgeCount_(0) {}

    explicit Graph(std::initializer_list<value_type> vertices) : Graph() {
        for (const value_type& vertex : vertices) {
            addVertex(vertex);
        }
    }

    Graph(std::initializer_list<value_type> vertices,
          std::initializer_list<edge_type> edges)
        : Graph(vertices) {
        for (const edge_type& edge : edges) {
            addEdge(edge.first, edge.second);
        }
    }

    Graph(const Graph& other) : Graph() {
        try {
            for (const value_type& vertex : other) {
                addVertex(vertex);
            }
            for (const edge_type& edge : other.edges()) {
                addEdge(edge.first, edge.second);
            }
        } catch (...) {
            clear();
            throw;
        }
    }

    Graph(Graph&& other) noexcept
        : verticesHead_(other.verticesHead_),
          verticesTail_(other.verticesTail_),
          edgesHead_(other.edgesHead_),
          edgesTail_(other.edgesTail_),
          vertexCount_(other.vertexCount_),
          edgeCount_(other.edgeCount_) {
        other.resetPointers();
    }

    ~Graph() { clear(); }

    Graph& operator=(const Graph& other) {
        if (this != &other) {
            Graph copy(other);
            swap(copy);
        }
        return *this;
    }

    Graph& operator=(Graph&& other) noexcept {
        if (this != &other) {
            clear();
            verticesHead_ = other.verticesHead_;
            verticesTail_ = other.verticesTail_;
            edgesHead_ = other.edgesHead_;
            edgesTail_ = other.edgesTail_;
            vertexCount_ = other.vertexCount_;
            edgeCount_ = other.edgeCount_;
            other.resetPointers();
        }
        return *this;
    }

    void swap(Graph& other) noexcept {
        std::swap(verticesHead_, other.verticesHead_);
        std::swap(verticesTail_, other.verticesTail_);
        std::swap(edgesHead_, other.edgesHead_);
        std::swap(edgesTail_, other.edgesTail_);
        std::swap(vertexCount_, other.vertexCount_);
        std::swap(edgeCount_, other.edgeCount_);
    }

    friend void swap(Graph& left, Graph& right) noexcept { left.swap(right); }

    bool empty() const noexcept { return vertexCount_ == 0; }
    size_type vertexCount() const noexcept { return vertexCount_; }
    size_type edgeCount() const noexcept { return edgeCount_; }

    void clear() noexcept {
        EdgeNode* edge = edgesHead_;
        while (edge != nullptr) {
            EdgeNode* next = edge->nextGlobal;
            delete edge;
            edge = next;
        }

        VertexNode* vertex = verticesHead_;
        while (vertex != nullptr) {
            VertexNode* next = vertex->next;
            delete vertex;
            vertex = next;
        }

        resetPointers();
    }

    bool containsVertex(const value_type& value) const {
        return findVertexNode(value) != nullptr;
    }

    bool containsEdge(const value_type& from, const value_type& to) const {
        VertexNode* source = findVertexNode(from);
        VertexNode* target = findVertexNode(to);
        if (source == nullptr || target == nullptr) {
            return false;
        }
        return findEdgeNode(source, target) != nullptr;
    }

    size_type degree(const value_type& value) const {
        return degreeOfNode(requireVertexNode(value));
    }

    edge_degree_type edgeDegree(const value_type& from,
                                const value_type& to) const {
        VertexNode* source = requireVertexNode(from);
        VertexNode* target = requireVertexNode(to);
        if (findEdgeNode(source, target) == nullptr) {
            throw EdgeNotFoundException("Cannot compute degree of a missing edge");
        }
        return edge_degree_type(degreeOfNode(source), degreeOfNode(target));
    }

    iterator findVertex(const value_type& value) {
        return iterator(this, findVertexNode(value));
    }

    const_iterator findVertex(const value_type& value) const {
        return const_iterator(this, findVertexNode(value));
    }

    EdgeIterator findEdge(const value_type& from, const value_type& to) {
        VertexNode* source = findVertexNode(from);
        VertexNode* target = findVertexNode(to);
        if (source == nullptr || target == nullptr) {
            return edgesEnd();
        }
        return EdgeIterator(this, findEdgeNode(source, target));
    }

    ConstEdgeIterator findEdge(const value_type& from,
                               const value_type& to) const {
        VertexNode* source = findVertexNode(from);
        VertexNode* target = findVertexNode(to);
        if (source == nullptr || target == nullptr) {
            return edgesEnd();
        }
        return ConstEdgeIterator(this, findEdgeNode(source, target));
    }

    iterator addVertex(const value_type& value) {
        if (findVertexNode(value) != nullptr) {
            throw DuplicateVertexException("Cannot add an existing vertex");
        }
        VertexNode* vertex = new VertexNode(value);
        appendVertexNode(vertex);
        return iterator(this, vertex);
    }

    iterator addVertex(value_type&& value) {
        if (findVertexNode(value) != nullptr) {
            throw DuplicateVertexException("Cannot add an existing vertex");
        }
        VertexNode* vertex = new VertexNode(std::move(value));
        appendVertexNode(vertex);
        return iterator(this, vertex);
    }

    EdgeIterator addEdge(const value_type& from, const value_type& to) {
        VertexNode* source = requireVertexNode(from);
        VertexNode* target = requireVertexNode(to);

        if (source == target) {
            throw LoopNotSupportedException("Undirected graph has no self-loops");
        }
        if (findEdgeNode(source, target) != nullptr) {
            throw DuplicateEdgeException("Cannot add an existing edge");
        }

        EdgeNode* edge = new EdgeNode(source, target);
        linkIncidentFront(source, edge);
        linkIncidentFront(target, edge);
        appendEdgeNode(edge);
        return EdgeIterator(this, edge);
    }

    size_type removeVertex(const value_type& value) {
        return eraseVertexNode(requireVertexNode(value));
    }

    size_type removeEdge(const value_type& from, const value_type& to) {
        VertexNode* source = requireVertexNode(from);
        VertexNode* target = requireVertexNode(to);
        EdgeNode* edge = findEdgeNode(source, target);
        if (edge == nullptr) {
            throw EdgeNotFoundException("Cannot remove a missing edge");
        }
        return eraseEdgeNode(edge);
    }

    iterator eraseVertex(iterator position) {
        validateVertexIterator(position.graph_, position.node_);
        VertexNode* next = position.node_->next;
        eraseVertexNode(position.node_);
        return iterator(this, next);
    }

    iterator eraseVertex(const_iterator position) {
        validateVertexIterator(position.graph_, position.node_);
        VertexNode* next = position.node_->next;
        eraseVertexNode(position.node_);
        return iterator(this, next);
    }

    EdgeIterator eraseEdge(EdgeIterator position) {
        validateEdgeIterator(position.graph_, position.node_);
        EdgeNode* next = position.node_->nextGlobal;
        eraseEdgeNode(position.node_);
        return EdgeIterator(this, next);
    }

    EdgeIterator eraseEdge(ConstEdgeIterator position) {
        validateEdgeIterator(position.graph_, position.node_);
        EdgeNode* next = position.node_->nextGlobal;
        eraseEdgeNode(position.node_);
        return EdgeIterator(this, next);
    }

    iterator begin() noexcept { return iterator(this, verticesHead_); }
    iterator end() noexcept { return iterator(this, nullptr); }
    const_iterator begin() const noexcept {
        return const_iterator(this, verticesHead_);
    }
    const_iterator end() const noexcept {
        return const_iterator(this, nullptr);
    }
    const_iterator cbegin() const noexcept {
        return const_iterator(this, verticesHead_);
    }
    const_iterator cend() const noexcept {
        return const_iterator(this, nullptr);
    }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }
    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }
    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

    EdgeIterator edgesBegin() noexcept { return EdgeIterator(this, edgesHead_); }
    EdgeIterator edgesEnd() noexcept { return EdgeIterator(this, nullptr); }
    ConstEdgeIterator edgesBegin() const noexcept {
        return ConstEdgeIterator(this, edgesHead_);
    }
    ConstEdgeIterator edgesEnd() const noexcept {
        return ConstEdgeIterator(this, nullptr);
    }
    ConstEdgeIterator cEdgesBegin() const noexcept {
        return ConstEdgeIterator(this, edgesHead_);
    }
    ConstEdgeIterator cEdgesEnd() const noexcept {
        return ConstEdgeIterator(this, nullptr);
    }

    reverse_edge_iterator edgesRBegin() noexcept {
        return reverse_edge_iterator(edgesEnd());
    }
    reverse_edge_iterator edgesREnd() noexcept {
        return reverse_edge_iterator(edgesBegin());
    }
    const_reverse_edge_iterator edgesRBegin() const noexcept {
        return const_reverse_edge_iterator(edgesEnd());
    }
    const_reverse_edge_iterator edgesREnd() const noexcept {
        return const_reverse_edge_iterator(edgesBegin());
    }
    const_reverse_edge_iterator cEdgesRBegin() const noexcept {
        return const_reverse_edge_iterator(cEdgesEnd());
    }
    const_reverse_edge_iterator cEdgesREnd() const noexcept {
        return const_reverse_edge_iterator(cEdgesBegin());
    }

    IncidentEdgeIterator incidentBegin(const value_type& value) {
        VertexNode* vertex = requireVertexNode(value);
        return IncidentEdgeIterator(this, vertex, vertex->edgesHead);
    }

    IncidentEdgeIterator incidentEnd(const value_type& value) {
        VertexNode* vertex = requireVertexNode(value);
        return IncidentEdgeIterator(this, vertex, nullptr);
    }

    ConstIncidentEdgeIterator incidentBegin(const value_type& value) const {
        VertexNode* vertex = requireVertexNode(value);
        return ConstIncidentEdgeIterator(this, vertex, vertex->edgesHead);
    }

    ConstIncidentEdgeIterator incidentEnd(const value_type& value) const {
        VertexNode* vertex = requireVertexNode(value);
        return ConstIncidentEdgeIterator(this, vertex, nullptr);
    }

    reverse_incident_edge_iterator incidentRBegin(const value_type& value) {
        return reverse_incident_edge_iterator(incidentEnd(value));
    }

    reverse_incident_edge_iterator incidentREnd(const value_type& value) {
        return reverse_incident_edge_iterator(incidentBegin(value));
    }

    const_reverse_incident_edge_iterator incidentRBegin(
        const value_type& value) const {
        return const_reverse_incident_edge_iterator(incidentEnd(value));
    }

    const_reverse_incident_edge_iterator incidentREnd(
        const value_type& value) const {
        return const_reverse_incident_edge_iterator(incidentBegin(value));
    }

    AdjacentVertexIterator adjacentBegin(const value_type& value) {
        VertexNode* vertex = requireVertexNode(value);
        return AdjacentVertexIterator(this, vertex, vertex->edgesHead);
    }

    AdjacentVertexIterator adjacentEnd(const value_type& value) {
        VertexNode* vertex = requireVertexNode(value);
        return AdjacentVertexIterator(this, vertex, nullptr);
    }

    ConstAdjacentVertexIterator adjacentBegin(const value_type& value) const {
        VertexNode* vertex = requireVertexNode(value);
        return ConstAdjacentVertexIterator(this, vertex, vertex->edgesHead);
    }

    ConstAdjacentVertexIterator adjacentEnd(const value_type& value) const {
        VertexNode* vertex = requireVertexNode(value);
        return ConstAdjacentVertexIterator(this, vertex, nullptr);
    }

    reverse_adjacent_vertex_iterator adjacentRBegin(const value_type& value) {
        return reverse_adjacent_vertex_iterator(adjacentEnd(value));
    }

    reverse_adjacent_vertex_iterator adjacentREnd(const value_type& value) {
        return reverse_adjacent_vertex_iterator(adjacentBegin(value));
    }

    const_reverse_adjacent_vertex_iterator adjacentRBegin(
        const value_type& value) const {
        return const_reverse_adjacent_vertex_iterator(adjacentEnd(value));
    }

    const_reverse_adjacent_vertex_iterator adjacentREnd(
        const value_type& value) const {
        return const_reverse_adjacent_vertex_iterator(adjacentBegin(value));
    }

    ForwardAdjacentVertexIterator forwardAdjacentBegin(
        const value_type& value) {
        VertexNode* vertex = requireVertexNode(value);
        return ForwardAdjacentVertexIterator(this, vertex, vertex->edgesHead);
    }

    ForwardAdjacentVertexIterator forwardAdjacentEnd(const value_type& value) {
        VertexNode* vertex = requireVertexNode(value);
        return ForwardAdjacentVertexIterator(this, vertex, nullptr);
    }

    ConstForwardAdjacentVertexIterator forwardAdjacentBegin(
        const value_type& value) const {
        VertexNode* vertex = requireVertexNode(value);
        return ConstForwardAdjacentVertexIterator(this, vertex,
                                                  vertex->edgesHead);
    }

    ConstForwardAdjacentVertexIterator forwardAdjacentEnd(
        const value_type& value) const {
        VertexNode* vertex = requireVertexNode(value);
        return ConstForwardAdjacentVertexIterator(this, vertex, nullptr);
    }

    bool operator==(const Graph& other) const {
        if (vertexCount_ != other.vertexCount_ || edgeCount_ != other.edgeCount_) {
            return false;
        }

        for (const value_type& vertex : *this) {
            if (!other.containsVertex(vertex)) {
                return false;
            }
        }
        for (const edge_type& edge : edges()) {
            if (!other.containsEdge(edge.first, edge.second)) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Graph& other) const { return !(*this == other); }
    bool operator<(const Graph& other) const { return compareTo(other) < 0; }
    bool operator>(const Graph& other) const { return compareTo(other) > 0; }
    bool operator<=(const Graph& other) const { return compareTo(other) <= 0; }
    bool operator>=(const Graph& other) const { return compareTo(other) >= 0; }

    friend std::ostream& operator<<(std::ostream& stream, const Graph& graph) {
        stream << "Graph{vertices=[";
        bool first = true;
        std::for_each(graph.cbegin(), graph.cend(),
                      [&stream, &first](const value_type& vertex) {
                          if (!first) {
                              stream << ", ";
                          }
                          stream << vertex;
                          first = false;
                      });
        stream << "], edges=[";
        first = true;
        std::for_each(
            graph.cEdgesBegin(), graph.cEdgesEnd(),
            [&stream, &first](const edge_type& edge) {
                if (!first) {
                    stream << ", ";
                }
                stream << "(" << edge.first << ", " << edge.second << ")";
                first = false;
            });
        stream << "]}";
        return stream;
    }

private:
    struct EdgeRange {
        const Graph* graph;

        ConstEdgeIterator begin() const { return graph->cEdgesBegin(); }
        ConstEdgeIterator end() const { return graph->cEdgesEnd(); }
    };

    EdgeRange edges() const { return EdgeRange{this}; }

    VertexNode* findVertexNode(const value_type& value) const {
        for (VertexNode* vertex = verticesHead_; vertex != nullptr;
             vertex = vertex->next) {
            if (vertex->value == value) {
                return vertex;
            }
        }
        return nullptr;
    }

    EdgeNode* findEdgeNode(const VertexNode* from,
                           const VertexNode* to) const noexcept {
        for (EdgeNode* edge = from->edgesHead; edge != nullptr;
             edge = nextIncident(edge, from)) {
            VertexNode* other = edge->from == from ? edge->to : edge->from;
            if (other == to) {
                return edge;
            }
        }
        return nullptr;
    }

    VertexNode* requireVertexNode(const value_type& value) const {
        VertexNode* vertex = findVertexNode(value);
        if (vertex == nullptr) {
            throw VertexNotFoundException("Cannot find graph vertex");
        }
        return vertex;
    }

    void validateVertexIterator(const Graph* graph, const VertexNode* node) const {
        if (graph != this || node == nullptr) {
            throw InvalidIteratorException("Invalid vertex iterator");
        }
    }

    void validateEdgeIterator(const Graph* graph, const EdgeNode* node) const {
        if (graph != this || node == nullptr) {
            throw InvalidIteratorException("Invalid edge iterator");
        }
    }

    void appendVertexNode(VertexNode* vertex) noexcept {
        vertex->next = nullptr;
        vertex->prev = verticesTail_;
        if (verticesTail_ != nullptr) {
            verticesTail_->next = vertex;
        } else {
            verticesHead_ = vertex;
        }
        verticesTail_ = vertex;
        ++vertexCount_;
    }

    void appendEdgeNode(EdgeNode* edge) noexcept {
        edge->nextGlobal = nullptr;
        edge->prevGlobal = edgesTail_;
        if (edgesTail_ != nullptr) {
            edgesTail_->nextGlobal = edge;
        } else {
            edgesHead_ = edge;
        }
        edgesTail_ = edge;
        ++edgeCount_;
    }

    void unlinkVertexNode(VertexNode* vertex) noexcept {
        if (vertex->prev != nullptr) {
            vertex->prev->next = vertex->next;
        } else {
            verticesHead_ = vertex->next;
        }
        if (vertex->next != nullptr) {
            vertex->next->prev = vertex->prev;
        } else {
            verticesTail_ = vertex->prev;
        }
        --vertexCount_;
    }

    void unlinkEdgeNode(EdgeNode* edge) noexcept {
        if (edge->prevGlobal != nullptr) {
            edge->prevGlobal->nextGlobal = edge->nextGlobal;
        } else {
            edgesHead_ = edge->nextGlobal;
        }
        if (edge->nextGlobal != nullptr) {
            edge->nextGlobal->prevGlobal = edge->prevGlobal;
        } else {
            edgesTail_ = edge->prevGlobal;
        }
        --edgeCount_;
    }

    size_type eraseEdgeNode(EdgeNode* edge) noexcept {
        unlinkIncident(edge->from, edge);
        unlinkIncident(edge->to, edge);
        unlinkEdgeNode(edge);
        delete edge;
        return 1;
    }

    size_type eraseVertexNode(VertexNode* vertex) noexcept {
        size_type removedEdges = 0;
        while (vertex->edgesHead != nullptr) {
            EdgeNode* edge = vertex->edgesHead;
            VertexNode* other =
                edge->from == vertex ? edge->to : edge->from;
            unlinkIncident(vertex, edge);
            unlinkIncident(other, edge);
            unlinkEdgeNode(edge);
            delete edge;
            ++removedEdges;
        }
        unlinkVertexNode(vertex);
        delete vertex;
        return removedEdges;
    }

    void resetPointers() noexcept {
        verticesHead_ = nullptr;
        verticesTail_ = nullptr;
        edgesHead_ = nullptr;
        edgesTail_ = nullptr;
        vertexCount_ = 0;
        edgeCount_ = 0;
    }

    static size_type indexOf(const std::vector<const value_type*>& vertices,
                             const value_type& value) {
        size_type low = 0;
        size_type high = vertices.size();
        while (low < high) {
            size_type middle = low + (high - low) / 2;
            if (*vertices[middle] < value) {
                low = middle + 1;
            } else {
                high = middle;
            }
        }
        return low;
    }

    std::vector<const value_type*> sortedVertexPointers() const {
        std::vector<const value_type*> vertices;
        vertices.reserve(vertexCount_);
        for (const value_type& vertex : *this) {
            vertices.push_back(&vertex);
        }
        std::sort(vertices.begin(), vertices.end(),
                  [](const value_type* left, const value_type* right) {
                      return *left < *right;
                  });
        return vertices;
    }

    std::vector<std::pair<size_type, size_type>> sortedEdgeIndices(
        const std::vector<const value_type*>& vertices) const {
        std::vector<std::pair<size_type, size_type>> indices;
        indices.reserve(edgeCount_);
        for (const edge_type& edge : edges()) {
            size_type from = indexOf(vertices, edge.first);
            size_type to = indexOf(vertices, edge.second);
            if (from > to) {
                std::swap(from, to);
            }
            indices.emplace_back(from, to);
        }
        std::sort(indices.begin(), indices.end());
        return indices;
    }

    int compareTo(const Graph& other) const {
        const std::vector<const value_type*> leftVertices =
            sortedVertexPointers();
        const std::vector<const value_type*> rightVertices =
            other.sortedVertexPointers();

        const size_type common =
            std::min(leftVertices.size(), rightVertices.size());
        for (size_type index = 0; index < common; ++index) {
            if (*leftVertices[index] < *rightVertices[index]) {
                return -1;
            }
            if (*rightVertices[index] < *leftVertices[index]) {
                return 1;
            }
        }
        if (leftVertices.size() < rightVertices.size()) {
            return -1;
        }
        if (leftVertices.size() > rightVertices.size()) {
            return 1;
        }

        const std::vector<std::pair<size_type, size_type>> leftEdges =
            sortedEdgeIndices(leftVertices);
        const std::vector<std::pair<size_type, size_type>> rightEdges =
            other.sortedEdgeIndices(rightVertices);
        if (leftEdges < rightEdges) {
            return -1;
        }
        if (rightEdges < leftEdges) {
            return 1;
        }
        return 0;
    }

    VertexNode* verticesHead_;
    VertexNode* verticesTail_;
    EdgeNode* edgesHead_;
    EdgeNode* edgesTail_;
    size_type vertexCount_;
    size_type edgeCount_;
};

}  // namespace graph

#endif  // GRAPH_GRAPH_H
