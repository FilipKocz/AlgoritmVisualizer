#ifndef ALGOVISUALIZER_H
#define ALGOVISUALIZER_H

#include <QMainWindow>
#include <vector>
#include "avl_tree.h"
#include "rb_tree.h"
#include <functional>
#include <map>

QT_BEGIN_NAMESPACE
namespace Ui { class AlgoVisualizer; }
QT_END_NAMESPACE

class AlgoVisualizer : public QMainWindow
{
    Q_OBJECT

public:
    AlgoVisualizer(QWidget *parent = nullptr);
    ~AlgoVisualizer();

    void renderArray(const std::vector<int>& data);

private slots:
    void on_sortButton_clicked();
    void on_treeInsertButton_clicked();
    void on_treeRemoveButton_clicked();
    void on_treeSearchButton_clicked();
    void on_treeClearButton_clicked();
    void on_treeTypeSelector_currentIndexChanged(int index);

private:
    Ui::AlgoVisualizer *ui;

    AVLTree avlTree;
    RBTree  rbTree;

    void renderTree();
    void renderAVL();
    void renderRB();
    void drawTree(QPainter& painter,
                  const std::vector<std::pair<int,int>>& positions, // key -> (x,y)
                  const std::function<void(QPainter&, int key, int x, int y)>& drawNode,
                  const std::vector<std::pair<int,int>>& edges, // parent key -> child key
                  const std::map<int,std::pair<int,int>>& keyToPos);
    void updateTreeStats(const std::string& lastOp, int rotations, int comparisons,
                         int nodeCount, int height);
};
#endif
