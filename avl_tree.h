#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <vector>
#include <string>
#include <functional>

struct AVLNode {
    int key;
    int height;
    AVLNode* left;
    AVLNode* right;
    AVLNode(int k) : key(k), height(1), left(nullptr), right(nullptr) {}
};

class AVLTree {
public:
    AVLTree();
    ~AVLTree();

    void insert(int key);
    void remove(int key);
    bool search(int key) const;
    void clear();

    // Dla wizualizacji - zwraca węzły jako listę (BFS) z pozycjami
    struct NodeInfo {
        int key;
        int height;
        int depth;
        int pos; // pozycja w poziomie (indeks w BFS danego poziomu)
        bool isLeft;
        int parentKey;
        bool hasParent;
    };
    std::vector<NodeInfo> getNodesForDrawing() const;
    int getHeight() const;
    int getNodeCount() const;

    // Statystyki ostatniej operacji
    struct OpStats {
        int rotations = 0;
        int comparisons = 0;
        std::string lastOp;
    };
    OpStats lastStats;

private:
    AVLNode* root;

    int height(AVLNode* n) const;
    int balanceFactor(AVLNode* n) const;
    AVLNode* rotateRight(AVLNode* y);
    AVLNode* rotateLeft(AVLNode* x);
    AVLNode* balance(AVLNode* n);
    AVLNode* insert(AVLNode* n, int key);
    AVLNode* minNode(AVLNode* n) const;
    AVLNode* remove(AVLNode* n, int key);
    void destroy(AVLNode* n);
    void collectNodes(AVLNode* n, int depth, int pos, int parentKey, bool hasParent, bool isLeft,
                      std::vector<NodeInfo>& result) const;
    int nodeCount(AVLNode* n) const;
};

#endif // AVL_TREE_H
