#include "graph.h"
#include <algorithm>
#include <queue>
#include <stack>
#include <cmath>
#include <random>
#include <functional>

Graph::Graph() : nextId(0) {}

void Graph::addNode(int id, double x, double y, const std::string& label) {
    if (!hasNode(id)) {
        nodes.push_back({id, x, y, label.empty() ? std::to_string(id) : label});
        if (id >= nextId) nextId = id + 1;
    }
}

void Graph::addEdge(int from, int to, int weight, bool directed) {
    if (hasNode(from) && hasNode(to) && !hasEdge(from, to))
        edges.push_back({from, to, weight, directed});
}

void Graph::removeNode(int id) {
    nodes.erase(std::remove_if(nodes.begin(), nodes.end(),
                               [id](const GraphNode& n){ return n.id == id; }), nodes.end());
    edges.erase(std::remove_if(edges.begin(), edges.end(),
                               [id](const GraphEdge& e){ return e.from == id || e.to == id; }), edges.end());
}

void Graph::removeEdge(int from, int to) {
    edges.erase(std::remove_if(edges.begin(), edges.end(),
                               [from, to](const GraphEdge& e){
                                   return (e.from == from && e.to == to) ||
                                          (!e.directed && e.from == to && e.to == from);
                               }), edges.end());
}

void Graph::clear() {
    nodes.clear();
    edges.clear();
    nextId = 0;
}

bool Graph::hasNode(int id) const {
    return std::any_of(nodes.begin(), nodes.end(),
                       [id](const GraphNode& n){ return n.id == id; });
}

bool Graph::hasEdge(int from, int to) const {
    return std::any_of(edges.begin(), edges.end(),
                       [from, to](const GraphEdge& e){
                           return (e.from == from && e.to == to) ||
                                  (!e.directed && e.from == to && e.to == from);
                       });
}

GraphNode* Graph::findNode(int id) {
    for (auto& n : nodes) if (n.id == id) return &n;
    return nullptr;
}

const GraphNode* Graph::findNode(int id) const {
    for (const auto& n : nodes) if (n.id == id) return &n;
    return nullptr;
}

std::vector<int> Graph::getNeighbors(int id) const {
    std::vector<int> result;
    for (const auto& e : edges) {
        if (e.from == id) result.push_back(e.to);
        else if (!e.directed && e.to == id) result.push_back(e.from);
    }
    return result;
}

int Graph::getWeight(int from, int to) const {
    for (const auto& e : edges) {
        if (e.from == from && e.to == to) return e.weight;
        if (!e.directed && e.from == to && e.to == from) return e.weight;
    }
    return 1;
}

// ============================================================
//  BFS
// ============================================================
GraphAlgoResult Graph::bfs(int startId) {
    GraphAlgoResult result;
    result.description = "BFS od węzła " + std::to_string(startId);
    if (!hasNode(startId)) return result;

    std::map<int, bool> visited;
    std::queue<int> q;
    visited[startId] = true;
    q.push(startId);
    result.parent[startId] = -1;

    while (!q.empty()) {
        int curr = q.front(); q.pop();
        result.visitOrder.push_back(curr);

        for (int nb : getNeighbors(curr)) {
            result.comparisons++;
            if (!visited[nb]) {
                visited[nb] = true;
                result.parent[nb] = curr;
                result.treeEdges.push_back({curr, nb});
                q.push(nb);
            }
        }
    }
    return result;
}

// ============================================================
//  DFS
// ============================================================
GraphAlgoResult Graph::dfs(int startId) {
    GraphAlgoResult result;
    result.description = "DFS od węzła " + std::to_string(startId);
    if (!hasNode(startId)) return result;

    std::map<int, bool> visited;
    std::function<void(int, int)> dfsRec = [&](int curr, int parent) {
        visited[curr] = true;
        result.visitOrder.push_back(curr);
        result.parent[curr] = parent;
        for (int nb : getNeighbors(curr)) {
            result.comparisons++;
            if (!visited[nb]) {
                result.treeEdges.push_back({curr, nb});
                dfsRec(nb, curr);
            }
        }
    };
    dfsRec(startId, -1);
    return result;
}

// ============================================================
//  DIJKSTRA
// ============================================================
GraphAlgoResult Graph::dijkstra(int startId) {
    GraphAlgoResult result;
    result.description = "Dijkstra od węzła " + std::to_string(startId);
    if (!hasNode(startId)) return result;

    const int INF = std::numeric_limits<int>::max();
    std::map<int, int> dist;
    for (const auto& n : nodes) dist[n.id] = INF;
    dist[startId] = 0;
    result.parent[startId] = -1;

    // min-heap: (dystans, id)
    std::priority_queue<std::pair<int,int>,
                        std::vector<std::pair<int,int>>,
                        std::greater<>> pq;
    pq.push({0, startId});

    std::set<int> settled;

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (settled.count(u)) continue;
        settled.insert(u);
        result.visitOrder.push_back(u);

        for (int v : getNeighbors(u)) {
            result.comparisons++;
            int w = getWeight(u, v);
            if (dist[u] != INF && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                result.parent[v] = u;
                result.treeEdges.push_back({u, v});
                result.relaxations++;
                pq.push({dist[v], v});
            }
        }
    }
    result.dist = dist;
    return result;
}

// ============================================================
//  PRIM MST
// ============================================================
GraphAlgoResult Graph::primMST() {
    GraphAlgoResult result;
    result.description = "Prim MST";
    if (nodes.empty()) return result;

    const int INF = std::numeric_limits<int>::max();
    std::map<int, int> key, parent;
    std::map<int, bool> inMST;

    for (const auto& n : nodes) { key[n.id] = INF; inMST[n.id] = false; }
    int start = nodes[0].id;
    key[start] = 0;
    result.parent[start] = -1;

    std::priority_queue<std::pair<int,int>,
                        std::vector<std::pair<int,int>>,
                        std::greater<>> pq;
    pq.push({0, start});

    while (!pq.empty()) {
        int u = pq.top().second; pq.pop();
        if (inMST[u]) continue;
        inMST[u] = true;
        result.visitOrder.push_back(u);

        for (int v : getNeighbors(u)) {
            result.comparisons++;
            int w = getWeight(u, v);
            if (!inMST[v] && w < key[v]) {
                key[v] = w;
                result.parent[v] = u;
                pq.push({w, v});
            }
        }
    }

    for (const auto& [node, par] : result.parent) {
        if (par != -1) result.treeEdges.push_back({par, node});
    }
    return result;
}

// ============================================================
//  KRUSKAL MST
// ============================================================
GraphAlgoResult Graph::kruskalMST() {
    GraphAlgoResult result;
    result.description = "Kruskal MST";
    if (nodes.empty()) return result;

    // Union-Find
    std::map<int, int> parent, rank;
    for (const auto& n : nodes) { parent[n.id] = n.id; rank[n.id] = 0; }

    std::function<int(int)> find = [&](int x) -> int {
        if (parent[x] != x) parent[x] = find(parent[x]);
        return parent[x];
    };
    auto unite = [&](int x, int y) {
        int px = find(x), py = find(y);
        if (px == py) return false;
        if (rank[px] < rank[py]) std::swap(px, py);
        parent[py] = px;
        if (rank[px] == rank[py]) rank[px]++;
        return true;
    };

    // Posortuj krawędzie po wadze
    std::vector<GraphEdge> sorted = edges;
    std::sort(sorted.begin(), sorted.end(),
              [](const GraphEdge& a, const GraphEdge& b){ return a.weight < b.weight; });

    for (const auto& e : sorted) {
        result.comparisons++;
        if (unite(e.from, e.to)) {
            result.treeEdges.push_back({e.from, e.to});
            result.visitOrder.push_back(e.from);
            result.visitOrder.push_back(e.to);
        }
    }
    return result;
}

// ============================================================
//  LOSOWY GRAF
// ============================================================
void Graph::generateRandom(int nodeCount, int edgeCount, bool directed) {
    clear();
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> posDist(0.05, 0.95);
    std::uniform_int_distribution<int> weightDist(1, 20);

    // Dodaj węzły w kole + lekki losowy offset
    for (int i = 0; i < nodeCount; ++i) {
        double angle = 2.0 * M_PI * i / nodeCount;
        double r = 0.35 + std::uniform_real_distribution<double>(-0.05, 0.05)(rng);
        double x = 0.5 + r * std::cos(angle);
        double y = 0.5 + r * std::sin(angle);
        addNode(i, x, y);
    }

    // Najpierw dodaj drzewo spinające żeby graf był spójny
    std::vector<int> ids;
    for (const auto& n : nodes) ids.push_back(n.id);
    std::shuffle(ids.begin(), ids.end(), rng);
    for (int i = 1; i < (int)ids.size(); ++i) {
        int w = weightDist(rng);
        addEdge(ids[i-1], ids[i], w, directed);
    }

    // Dodaj pozostałe losowe krawędzie
    int attempts = 0;
    int added = nodes.size() - 1;
    while (added < edgeCount && attempts < 1000) {
        int a = ids[std::uniform_int_distribution<int>(0, ids.size()-1)(rng)];
        int b = ids[std::uniform_int_distribution<int>(0, ids.size()-1)(rng)];
        if (a != b && !hasEdge(a, b)) {
            addEdge(a, b, weightDist(rng), directed);
            added++;
        }
        attempts++;
    }
}
