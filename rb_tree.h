#ifndef RB_TREE_H
#define RB_TREE_H

#include <vector>
#include <string>

enum RBColor { RED, BLACK };

struct RBNode {
    int key;
    RBColor color;
    RBNode* left;
    RBNode* right;
    RBNode* parent;
    RBNode(int k) : key(k), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};

class RBTree {
public:
    RBTree();
    ~RBTree();

    void insert(int key);
    void remove(int key);
    bool search(int key) const;
    void clear();

    struct NodeInfo {
        int key;
        RBColor color;
        int depth;
        int pos;
        bool isLeft;
        int parentKey;
        bool hasParent;
    };
    std::vector<NodeInfo> getNodesForDrawing() const;
    int getHeight() const;
    int getNodeCount() const;

    struct OpStats {
        int rotations = 0;
        int comparisons = 0;
        std::string lastOp;
    };
    OpStats lastStats;

private:
    RBNode* root;
    RBNode* NIL; // sentinel

    void rotateLeft(RBNode* x);
    void rotateRight(RBNode* x);
    void insertFixup(RBNode* z);
    void transplant(RBNode* u, RBNode* v);
    RBNode* minimum(RBNode* x) const;
    void deleteFixup(RBNode* x);
    void destroy(RBNode* n);
    int height(RBNode* n) const;
    int nodeCount(RBNode* n) const;
    void collectNodes(RBNode* n, int depth, int pos, RBNode* parent,
                      bool isLeft, std::vector<NodeInfo>& result) const;
};

#endif // RB_TREE_H
