#include "avl_tree.h"
#include <algorithm>
#include <queue>

AVLTree::AVLTree() : root(nullptr) {}

AVLTree::~AVLTree() { destroy(root); }

void AVLTree::destroy(AVLNode* n) {
    if (!n) return;
    destroy(n->left);
    destroy(n->right);
    delete n;
}

void AVLTree::clear() {
    destroy(root);
    root = nullptr;
}

int AVLTree::height(AVLNode* n) const { return n ? n->height : 0; }

int AVLTree::balanceFactor(AVLNode* n) const {
    return n ? height(n->left) - height(n->right) : 0;
}

AVLNode* AVLTree::rotateRight(AVLNode* y) {
    lastStats.rotations++;
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = std::max(height(y->left), height(y->right)) + 1;
    x->height = std::max(height(x->left), height(x->right)) + 1;
    return x;
}

AVLNode* AVLTree::rotateLeft(AVLNode* x) {
    lastStats.rotations++;
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = std::max(height(x->left), height(x->right)) + 1;
    y->height = std::max(height(y->left), height(y->right)) + 1;
    return y;
}

AVLNode* AVLTree::balance(AVLNode* n) {
    n->height = std::max(height(n->left), height(n->right)) + 1;
    int bf = balanceFactor(n);
    // LL
    if (bf > 1 && balanceFactor(n->left) >= 0) return rotateRight(n);
    // LR
    if (bf > 1 && balanceFactor(n->left) < 0) {
        n->left = rotateLeft(n->left);
        return rotateRight(n);
    }
    // RR
    if (bf < -1 && balanceFactor(n->right) <= 0) return rotateLeft(n);
    // RL
    if (bf < -1 && balanceFactor(n->right) > 0) {
        n->right = rotateRight(n->right);
        return rotateLeft(n);
    }
    return n;
}

AVLNode* AVLTree::insert(AVLNode* n, int key) {
    if (!n) return new AVLNode(key);
    lastStats.comparisons++;
    if (key < n->key) n->left = insert(n->left, key);
    else if (key > n->key) n->right = insert(n->right, key);
    else return n; // duplikat
    return balance(n);
}

void AVLTree::insert(int key) {
    lastStats = {0, 0, "Insert " + std::to_string(key)};
    root = insert(root, key);
}

AVLNode* AVLTree::minNode(AVLNode* n) const {
    return n->left ? minNode(n->left) : n;
}

AVLNode* AVLTree::remove(AVLNode* n, int key) {
    if (!n) return nullptr;
    lastStats.comparisons++;
    if (key < n->key) n->left = remove(n->left, key);
    else if (key > n->key) n->right = remove(n->right, key);
    else {
        if (!n->left || !n->right) {
            AVLNode* temp = n->left ? n->left : n->right;
            delete n;
            return temp;
        }
        AVLNode* temp = minNode(n->right);
        n->key = temp->key;
        n->right = remove(n->right, temp->key);
    }
    return balance(n);
}

void AVLTree::remove(int key) {
    lastStats = {0, 0, "Remove " + std::to_string(key)};
    root = remove(root, key);
}

bool AVLTree::search(int key) const {
    AVLNode* curr = root;
    while (curr) {
        if (key == curr->key) return true;
        curr = key < curr->key ? curr->left : curr->right;
    }
    return false;
}

int AVLTree::getHeight() const { return height(root); }

int AVLTree::nodeCount(AVLNode* n) const {
    return n ? 1 + nodeCount(n->left) + nodeCount(n->right) : 0;
}

int AVLTree::getNodeCount() const { return nodeCount(root); }

void AVLTree::collectNodes(AVLNode* n, int depth, int pos, int parentKey,
                           bool hasParent, bool isLeft,
                           std::vector<NodeInfo>& result) const {
    if (!n) return;
    result.push_back({n->key, n->height, depth, pos, isLeft, parentKey, hasParent});
    collectNodes(n->left,  depth+1, pos*2,   n->key, true, true,  result);
    collectNodes(n->right, depth+1, pos*2+1, n->key, true, false, result);
}

std::vector<AVLTree::NodeInfo> AVLTree::getNodesForDrawing() const {
    std::vector<NodeInfo> result;
    collectNodes(root, 0, 0, 0, false, false, result);
    return result;
}
