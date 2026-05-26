#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <map>
#include <set>
#include <limits>

struct GraphNode {
    int id;
    double x, y;   // pozycja na ekranie (0.0 - 1.0)
    std::string label;
};

struct GraphEdge {
    int from, to;
    int weight;
    bool directed;
};

struct GraphAlgoResult {
    std::vector<int> visitOrder;          // kolejność odwiedzania węzłów
    std::vector<std::pair<int,int>> treeEdges;  // krawędzie drzewa wynikowego
    std::map<int, int> parent;            // poprzednik w ścieżce
    std::map<int, int> dist;              // dystans od źródła
    std::string description;
    int comparisons = 0;
    int relaxations = 0;
};

class Graph {
public:
    Graph();

    void addNode(int id, double x, double y, const std::string& label = "");
    void addEdge(int from, int to, int weight = 1, bool directed = false);
    void removeNode(int id);
    void removeEdge(int from, int to);
    void clear();
    bool hasNode(int id) const;
    bool hasEdge(int from, int to) const;

    const std::vector<GraphNode>& getNodes() const { return nodes; }
    const std::vector<GraphEdge>& getEdges() const { return edges; }
    int getNodeCount() const { return nodes.size(); }

    // Algorytmy
    GraphAlgoResult bfs(int startId);
    GraphAlgoResult dfs(int startId);
    GraphAlgoResult dijkstra(int startId);
    GraphAlgoResult primMST();
    GraphAlgoResult kruskalMST();

    // Wylosuj graf
    void generateRandom(int nodeCount, int edgeCount, bool directed = false);

private:
    std::vector<GraphNode> nodes;
    std::vector<GraphEdge> edges;
    int nextId;

    std::vector<int> getNeighbors(int id) const;
    int getWeight(int from, int to) const;
    GraphNode* findNode(int id);
    const GraphNode* findNode(int id) const;
};

#endif // GRAPH_H
