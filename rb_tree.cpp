#include "rb_tree.h"
#include <algorithm>

RBTree::RBTree() {
    NIL = new RBNode(0);
    NIL->color = BLACK;
    NIL->left = NIL->right = NIL->parent = nullptr;
    root = NIL;
}

RBTree::~RBTree() {
    destroy(root);
    delete NIL;
}

void RBTree::destroy(RBNode* n) {
    if (n == NIL || !n) return;
    destroy(n->left);
    destroy(n->right);
    delete n;
}

void RBTree::clear() {
    destroy(root);
    root = NIL;
}

void RBTree::rotateLeft(RBNode* x) {
    lastStats.rotations++;
    RBNode* y = x->right;
    x->right = y->left;
    if (y->left != NIL) y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == nullptr) root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->left = x;
    x->parent = y;
}

void RBTree::rotateRight(RBNode* y) {
    lastStats.rotations++;
    RBNode* x = y->left;
    y->left = x->right;
    if (x->right != NIL) x->right->parent = y;
    x->parent = y->parent;
    if (y->parent == nullptr) root = x;
    else if (y == y->parent->right) y->parent->right = x;
    else y->parent->left = x;
    x->right = y;
    y->parent = x;
}

void RBTree::insertFixup(RBNode* z) {
    while (z->parent && z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            RBNode* y = z->parent->parent->right;
            if (y->color == RED) { // Case 1
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) { // Case 2
                    z = z->parent;
                    rotateLeft(z);
                }
                z->parent->color = BLACK; // Case 3
                z->parent->parent->color = RED;
                rotateRight(z->parent->parent);
            }
        } else {
            RBNode* y = z->parent->parent->left;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rotateRight(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rotateLeft(z->parent->parent);
            }
        }
    }
    root->color = BLACK;
}

void RBTree::insert(int key) {
    lastStats = {0, 0, "Insert " + std::to_string(key)};
    RBNode* z = new RBNode(key);
    z->left = z->right = z->parent = NIL;

    RBNode* y = nullptr;
    RBNode* x = root;
    while (x != NIL) {
        lastStats.comparisons++;
        y = x;
        if (z->key < x->key) x = x->left;
        else if (z->key > x->key) x = x->right;
        else { delete z; return; } // duplikat
    }
    z->parent = y;
    if (y == nullptr) root = z;
    else if (z->key < y->key) y->left = z;
    else y->right = z;

    insertFixup(z);
}

void RBTree::transplant(RBNode* u, RBNode* v) {
    if (u->parent == nullptr) root = v;
    else if (u == u->parent->left) u->parent->left = v;
    else u->parent->right = v;
    v->parent = u->parent;
}

RBNode* RBTree::minimum(RBNode* x) const {
    while (x->left != NIL) x = x->left;
    return x;
}

void RBTree::deleteFixup(RBNode* x) {
    while (x != root && x->color == BLACK) {
        if (x == x->parent->left) {
            RBNode* w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK; x->parent->color = RED;
                rotateLeft(x->parent); w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED; x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK; w->color = RED;
                    rotateRight(w); w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK; w->right->color = BLACK;
                rotateLeft(x->parent); x = root;
            }
        } else {
            RBNode* w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK; x->parent->color = RED;
                rotateRight(x->parent); w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED; x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK; w->color = RED;
                    rotateLeft(w); w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK; w->left->color = BLACK;
                rotateRight(x->parent); x = root;
            }
        }
    }
    x->color = BLACK;
}

void RBTree::remove(int key) {
    lastStats = {0, 0, "Remove " + std::to_string(key)};
    RBNode* z = root;
    while (z != NIL) {
        lastStats.comparisons++;
        if (key == z->key) break;
        z = key < z->key ? z->left : z->right;
    }
    if (z == NIL) return;

    RBNode* y = z; RBNode* x;
    RBColor yOrigColor = y->color;
    if (z->left == NIL) { x = z->right; transplant(z, z->right); }
    else if (z->right == NIL) { x = z->left; transplant(z, z->left); }
    else {
        y = minimum(z->right); yOrigColor = y->color; x = y->right;
        if (y->parent == z) x->parent = y;
        else { transplant(y, y->right); y->right = z->right; y->right->parent = y; }
        transplant(z, y); y->left = z->left; y->left->parent = y; y->color = z->color;
    }
    delete z;
    if (yOrigColor == BLACK) deleteFixup(x);
}

bool RBTree::search(int key) const {
    RBNode* curr = root;
    while (curr != NIL) {
        if (key == curr->key) return true;
        curr = key < curr->key ? curr->left : curr->right;
    }
    return false;
}

int RBTree::height(RBNode* n) const {
    if (n == NIL || !n) return 0;
    return 1 + std::max(height(n->left), height(n->right));
}

int RBTree::getHeight() const { return height(root); }

int RBTree::nodeCount(RBNode* n) const {
    if (n == NIL || !n) return 0;
    return 1 + nodeCount(n->left) + nodeCount(n->right);
}

int RBTree::getNodeCount() const { return nodeCount(root); }

void RBTree::collectNodes(RBNode* n, int depth, int pos, RBNode* parent,
                          bool isLeft, std::vector<NodeInfo>& result) const {
    if (n == NIL || !n) return;
    bool hasParent = (parent != nullptr && parent != NIL);
    int parentKey = hasParent ? parent->key : 0;
    result.push_back({n->key, n->color, depth, pos, isLeft, parentKey, hasParent});
    collectNodes(n->left,  depth+1, pos*2,   n, true,  result);
    collectNodes(n->right, depth+1, pos*2+1, n, false, result);
}

std::vector<RBTree::NodeInfo> RBTree::getNodesForDrawing() const {
    std::vector<NodeInfo> result;
    collectNodes(root, 0, 0, nullptr, false, result);
    return result;
}
