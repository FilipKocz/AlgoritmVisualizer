#ifndef ALGOVISUALIZER_H
#define ALGOVISUALIZER_H

#include <QMainWindow>
#include <vector>
#include <map>
#include "avl_tree.h"
#include "rb_tree.h"
#include "graph.h"
#include "maze.h"

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

    void on_graphGenerateButton_clicked();
    void on_graphAlgoButton_clicked();
    void on_graphClearButton_clicked();

    void on_mazeGenerateButton_clicked();
    void on_mazeSolveButton_clicked();

private:
    Ui::AlgoVisualizer *ui;

    AVLTree avlTree;
    RBTree  rbTree;
    void renderTree();
    void renderAVL();
    void renderRB();
    void updateTreeStats(const std::string& lastOp, int rotations,
                         int comparisons, int nodeCount, int height);

    Graph graph;
    GraphAlgoResult currentGraphResult;
    void renderGraph(const GraphAlgoResult* highlight = nullptr);

    Maze maze;
    MazeResult currentMazeResult;
    void renderMaze(const MazeResult* result = nullptr);
};

#endif // ALGOVISUALIZER_H
