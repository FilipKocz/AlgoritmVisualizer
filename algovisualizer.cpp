#include "algovisualizer.h"
#include "./ui_algovisualizer.h"
#include "sorter.h"
#include <QPainter>
#include <QRandomGenerator>
#include <chrono>
#include <QThread>
#include <map>
#include <set>
#include <cmath>

AlgoVisualizer::AlgoVisualizer(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::AlgoVisualizer)
{
    ui->setupUi(this);
    ui->resultLabel->setAlignment(Qt::AlignCenter);
    ui->resultLabel->setText("Wybierz parametry i naciśnij Sortuj");
    ui->treeCanvas->setAlignment(Qt::AlignCenter);
    ui->treeCanvas->setText("Drzewo jest puste. Wstaw węzeł.");
    ui->graphCanvas->setAlignment(Qt::AlignCenter);
    ui->graphCanvas->setText("Kliknij 'Generuj graf' aby zacząć.");
    ui->mazeCanvas->setAlignment(Qt::AlignCenter);
    ui->mazeCanvas->setText("Kliknij 'Generuj labirynt' aby zacząć.");
}

AlgoVisualizer::~AlgoVisualizer() { delete ui; }

// ================================================================
//  SORTING
// ================================================================
void AlgoVisualizer::renderArray(const std::vector<int>& data) {
    if (ui->resultLabel->width() <= 0 || ui->resultLabel->height() <= 0) return;
    QPixmap pixmap(ui->resultLabel->size());
    pixmap.fill(Qt::black);
    QPainter painter(&pixmap);
    int n = data.size();
    if (n == 0) return;
    int bottomSpace = 25;
    double barWidth = (double)pixmap.width() / n;
    int maxHeight = pixmap.height() - bottomSpace;
    QFont font = painter.font();
    font.setPixelSize(qBound(8, (int)(barWidth * 0.4), 12));
    painter.setFont(font);
    for (int i = 0; i < n; ++i) {
        int barHeight = (data[i] * (maxHeight - 20)) / 100;
        painter.setBrush(QColor(0, 180, 255));
        painter.setPen(QPen(Qt::black, 1));
        painter.drawRect(i * barWidth, maxHeight - barHeight, barWidth, barHeight);
        painter.setPen(Qt::white);
        painter.drawText(QRect(i * barWidth, maxHeight, barWidth, bottomSpace),
                         Qt::AlignCenter, QString::number(data[i]));
    }
    ui->resultLabel->setPixmap(pixmap);
    QCoreApplication::processEvents();
}

void AlgoVisualizer::on_sortButton_clicked() {
    std::vector<int> data;
    for (int i = 0; i < 10; ++i)
        data.push_back(QRandomGenerator::global()->bounded(5, 100));
    SortStats stats;
    bool viz = ui->visualizeCheckBox->isChecked();
    AlgoVisualizer* vPtr = viz ? this : nullptr;
    if (viz) { renderArray(data); QThread::msleep(200); }
    else { ui->resultLabel->setPixmap(QPixmap()); ui->resultLabel->setText("Sortowanie..."); }
    QString sel = ui->algoSelector->currentText();
    auto start = std::chrono::steady_clock::now();
    if      (sel == "Bubble Sort")    Sorter::bubbleSort(data, vPtr, stats);
    else if (sel == "Selection Sort") Sorter::selectionSort(data, vPtr, stats);
    else if (sel == "Insertion Sort") Sorter::insertionSort(data, vPtr, stats);
    else if (sel == "Quick Sort")     Sorter::quickSort(data, 0, data.size()-1, vPtr, stats);
    else if (sel == "Merge Sort")     Sorter::mergeSort(data, 0, data.size()-1, vPtr, stats);
    else if (sel == "Counting Sort")  Sorter::countingSort(data, vPtr, stats);
    else if (sel == "Bucket Sort")    Sorter::bucketSort(data, vPtr, stats);
    auto end = std::chrono::steady_clock::now();
    stats.durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    if (!viz) renderArray(data);
    ui->statusbar->showMessage(QString("%1 | Czas: %2 ms | Porównania: %3 | Zamiany: %4")
                                   .arg(sel).arg(stats.durationMs).arg(stats.comparisons).arg(stats.swaps));
}

// ================================================================
//  TREES
// ================================================================
void AlgoVisualizer::updateTreeStats(const std::string& op, int rot, int cmp, int cnt, int h) {
    ui->statusbar->showMessage(QString("Op: %1 | Rotacje: %2 | Porównania: %3 | Węzły: %4 | H: %5")
                                   .arg(QString::fromStdString(op)).arg(rot).arg(cmp).arg(cnt).arg(h));
}

void AlgoVisualizer::renderTree() {
    if (ui->treeTypeSelector->currentIndex() == 0) renderAVL();
    else renderRB();
}

void AlgoVisualizer::renderAVL() {
    auto nodes = avlTree.getNodesForDrawing();
    if (nodes.empty()) { ui->treeCanvas->setPixmap(QPixmap()); ui->treeCanvas->setText("Drzewo AVL jest puste."); return; }
    QPixmap pixmap(ui->treeCanvas->size());
    pixmap.fill(Qt::black);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    int w = pixmap.width(), h = pixmap.height();
    int treeH = avlTree.getHeight();
    int levelH = std::max(40, h / (treeH + 1));
    int nodeR = qBound(12, levelH / 3, 22);
    std::map<int, std::pair<int,int>> keyToPos;
    for (auto& n : nodes) {
        int maxPos = (1 << n.depth);
        double slotW = (double)w / maxPos;
        int cx = (int)(slotW * n.pos + slotW / 2);
        int cy = std::min(levelH * n.depth + levelH / 2, h - nodeR - 5);
        keyToPos[n.key] = {cx, cy};
    }
    painter.setPen(QPen(QColor(180,180,180), 1.5));
    for (auto& n : nodes)
        if (n.hasParent && keyToPos.count(n.parentKey)) {
            auto [px,py] = keyToPos[n.parentKey];
            auto [cx,cy] = keyToPos[n.key];
            painter.drawLine(px, py, cx, cy);
        }
    QFont font = painter.font(); font.setPixelSize(qBound(8, nodeR-2, 13)); font.setBold(true);
    painter.setFont(font);
    for (auto& n : nodes) {
        auto [cx,cy] = keyToPos[n.key];
        painter.setBrush(QColor(0,160,220)); painter.setPen(QPen(QColor(0,220,255),2));
        painter.drawEllipse(cx-nodeR, cy-nodeR, nodeR*2, nodeR*2);
        painter.setPen(Qt::white);
        painter.drawText(QRect(cx-nodeR, cy-nodeR, nodeR*2, nodeR*2), Qt::AlignCenter, QString::number(n.key));
    }
    ui->treeCanvas->setPixmap(pixmap);
}

void AlgoVisualizer::renderRB() {
    auto nodes = rbTree.getNodesForDrawing();
    if (nodes.empty()) { ui->treeCanvas->setPixmap(QPixmap()); ui->treeCanvas->setText("Drzewo RB jest puste."); return; }
    QPixmap pixmap(ui->treeCanvas->size());
    pixmap.fill(Qt::black);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    int w = pixmap.width(), h = pixmap.height();
    int treeH = rbTree.getHeight();
    int levelH = std::max(40, h / (treeH + 1));
    int nodeR = qBound(12, levelH / 3, 22);
    std::map<int, std::pair<int,int>> keyToPos;
    for (auto& n : nodes) {
        int maxPos = (1 << n.depth);
        double slotW = (double)w / maxPos;
        int cx = (int)(slotW * n.pos + slotW / 2);
        int cy = std::min(levelH * n.depth + levelH / 2, h - nodeR - 5);
        keyToPos[n.key] = {cx, cy};
    }
    painter.setPen(QPen(QColor(160,160,160), 1.5));
    for (auto& n : nodes)
        if (n.hasParent && keyToPos.count(n.parentKey)) {
            auto [px,py] = keyToPos[n.parentKey];
            auto [cx,cy] = keyToPos[n.key];
            painter.drawLine(px, py, cx, cy);
        }
    QFont font = painter.font(); font.setPixelSize(qBound(8, nodeR-2, 13)); font.setBold(true);
    painter.setFont(font);
    for (auto& n : nodes) {
        auto [cx,cy] = keyToPos[n.key];
        QColor fill = (n.color == RED) ? QColor(220,40,40) : QColor(50,50,50);
        QColor border = (n.color == RED) ? QColor(255,100,100) : QColor(140,140,140);
        painter.setBrush(fill); painter.setPen(QPen(border,2));
        painter.drawEllipse(cx-nodeR, cy-nodeR, nodeR*2, nodeR*2);
        painter.setPen(Qt::white);
        painter.drawText(QRect(cx-nodeR, cy-nodeR, nodeR*2, nodeR*2), Qt::AlignCenter, QString::number(n.key));
    }
    ui->treeCanvas->setPixmap(pixmap);
}

void AlgoVisualizer::on_treeTypeSelector_currentIndexChanged(int) { renderTree(); ui->statusbar->clearMessage(); }

void AlgoVisualizer::on_treeInsertButton_clicked() {
    bool ok; int val = ui->treeValueInput->text().toInt(&ok);
    if (!ok) { ui->statusbar->showMessage("Błąd: podaj liczbę całkowitą."); return; }
    if (ui->treeTypeSelector->currentIndex() == 0) {
        avlTree.insert(val);
        updateTreeStats(avlTree.lastStats.lastOp, avlTree.lastStats.rotations, avlTree.lastStats.comparisons, avlTree.getNodeCount(), avlTree.getHeight());
    } else {
        rbTree.insert(val);
        updateTreeStats(rbTree.lastStats.lastOp, rbTree.lastStats.rotations, rbTree.lastStats.comparisons, rbTree.getNodeCount(), rbTree.getHeight());
    }
    renderTree();
}

void AlgoVisualizer::on_treeRemoveButton_clicked() {
    bool ok; int val = ui->treeValueInput->text().toInt(&ok);
    if (!ok) { ui->statusbar->showMessage("Błąd: podaj liczbę całkowitą."); return; }
    if (ui->treeTypeSelector->currentIndex() == 0) {
        avlTree.remove(val);
        updateTreeStats(avlTree.lastStats.lastOp, avlTree.lastStats.rotations, avlTree.lastStats.comparisons, avlTree.getNodeCount(), avlTree.getHeight());
    } else {
        rbTree.remove(val);
        updateTreeStats(rbTree.lastStats.lastOp, rbTree.lastStats.rotations, rbTree.lastStats.comparisons, rbTree.getNodeCount(), rbTree.getHeight());
    }
    renderTree();
}

void AlgoVisualizer::on_treeSearchButton_clicked() {
    bool ok; int val = ui->treeValueInput->text().toInt(&ok);
    if (!ok) { ui->statusbar->showMessage("Błąd: podaj liczbę całkowitą."); return; }
    bool found = (ui->treeTypeSelector->currentIndex() == 0) ? avlTree.search(val) : rbTree.search(val);
    ui->statusbar->showMessage(found ? QString("Znaleziono: %1 ✓").arg(val) : QString("Nie znaleziono: %1 ✗").arg(val));
}

void AlgoVisualizer::on_treeClearButton_clicked() {
    if (ui->treeTypeSelector->currentIndex() == 0) avlTree.clear(); else rbTree.clear();
    renderTree(); ui->statusbar->showMessage("Drzewo wyczyszczone.");
}

// ================================================================
//  GRAPH
// ================================================================
void AlgoVisualizer::renderGraph(const GraphAlgoResult* res) {
    QLabel* canvas = ui->graphCanvas;
    if (canvas->width() <= 0) return;
    QPixmap pixmap(canvas->size());
    pixmap.fill(QColor(15, 15, 25));
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    int W = pixmap.width(), H = pixmap.height();
    int nodeR = 18;

    // Zbiór krawędzi podświetlonych
    std::set<std::pair<int,int>> highlighted;
    std::set<int> visitedNodes;
    if (res) {
        for (auto& e : res->treeEdges) {
            highlighted.insert({e.first, e.second});
            highlighted.insert({e.second, e.first});
        }
        for (int v : res->visitOrder) visitedNodes.insert(v);
    }

    // Rysuj krawędzie
    for (const auto& e : graph.getEdges()) {
        const GraphNode* A = nullptr; const GraphNode* B = nullptr;
        for (const auto& n : graph.getNodes()) {
            if (n.id == e.from) A = &n;
            if (n.id == e.to)   B = &n;
        }
        if (!A || !B) continue;
        int ax = A->x * W, ay = A->y * H;
        int bx = B->x * W, by = B->y * H;

        bool isHL = highlighted.count({e.from, e.to});
        painter.setPen(QPen(isHL ? QColor(255,200,0) : QColor(80,80,120), isHL ? 3 : 1.5));
        painter.drawLine(ax, ay, bx, by);

        // Waga
        int mx = (ax+bx)/2, my = (ay+by)/2;
        painter.setPen(QColor(200,200,200));
        QFont f = painter.font(); f.setPixelSize(10); painter.setFont(f);
        painter.drawText(QRect(mx-12, my-8, 24, 16), Qt::AlignCenter, QString::number(e.weight));

        // Strzałka dla grafów skierowanych
        if (e.directed) {
            double ddx = bx-ax, ddy = by-ay;
            double dlen = std::sqrt(ddx*ddx + ddy*ddy);
            if (dlen > 0) {
                ddx /= dlen; ddy /= dlen;
                int ex2 = bx - (int)(ddx*nodeR);
                int ey2 = by - (int)(ddy*nodeR);
                double px2 = -ddy*6, py2 = ddx*6;
                QPolygon arrow;
                arrow << QPoint(ex2, ey2)
                      << QPoint((int)(ex2 - ddx*12 + px2), (int)(ey2 - ddy*12 + py2))
                      << QPoint((int)(ex2 - ddx*12 - px2), (int)(ey2 - ddy*12 - py2));
                painter.setBrush(isHL ? QColor(255,200,0) : QColor(80,80,120));
                painter.drawPolygon(arrow);
            }
        }
    }

    // Rysuj węzły
    QFont nf = painter.font(); nf.setPixelSize(11); nf.setBold(true); painter.setFont(nf);
    for (const auto& n : graph.getNodes()) {
        int cx = n.x * W, cy = n.y * H;
        bool isVisited = visitedNodes.count(n.id);
        QColor fill  = isVisited ? QColor(255,160,0)   : QColor(40,100,180);
        QColor border= isVisited ? QColor(255,220,100) : QColor(100,160,255);
        painter.setBrush(fill);
        painter.setPen(QPen(border, 2));
        painter.drawEllipse(cx-nodeR, cy-nodeR, nodeR*2, nodeR*2);
        painter.setPen(Qt::white);
        painter.drawText(QRect(cx-nodeR, cy-nodeR, nodeR*2, nodeR*2),
                         Qt::AlignCenter, QString::fromStdString(n.label));
    }

    canvas->setPixmap(pixmap);
}

void AlgoVisualizer::on_graphGenerateButton_clicked() {
    int nodes = ui->graphNodeCount->value();
    int edges = ui->graphEdgeCount->value();
    bool directed = ui->graphDirectedCheck->isChecked();
    graph.generateRandom(nodes, edges, directed);
    currentGraphResult = GraphAlgoResult{};
    renderGraph();
    ui->statusbar->showMessage(QString("Graf wygenerowany: %1 węzłów, %2 krawędzi")
                                   .arg(graph.getNodeCount()).arg(graph.getEdges().size()));
}

void AlgoVisualizer::on_graphAlgoButton_clicked() {
    if (graph.getNodeCount() == 0) { ui->statusbar->showMessage("Najpierw wygeneruj graf."); return; }
    int startId = graph.getNodes().front().id;
    QString algo = ui->graphAlgoSelector->currentText();

    if      (algo == "BFS")      currentGraphResult = graph.bfs(startId);
    else if (algo == "DFS")      currentGraphResult = graph.dfs(startId);
    else if (algo == "Dijkstra") currentGraphResult = graph.dijkstra(startId);

    renderGraph(&currentGraphResult);
    ui->statusbar->showMessage(QString("%1 | Odwiedzone: %2 | Krawędzie drzewa: %3 | Porównania: %4")
                                   .arg(algo)
                                   .arg(currentGraphResult.visitOrder.size())
                                   .arg(currentGraphResult.treeEdges.size())
                                   .arg(currentGraphResult.comparisons));
}

void AlgoVisualizer::on_graphClearButton_clicked() {
    graph.clear();
    currentGraphResult = GraphAlgoResult{};
    ui->graphCanvas->setPixmap(QPixmap());
    ui->graphCanvas->setText("Graf wyczyszczony.");
    ui->statusbar->clearMessage();
}

// ================================================================
//  MAZE
// ================================================================
void AlgoVisualizer::renderMaze(const MazeResult* res) {
    QLabel* canvas = ui->mazeCanvas;
    if (canvas->width() <= 0 || maze.getCols() == 0) return;

    QPixmap pixmap(canvas->size());
    pixmap.fill(Qt::black);
    QPainter painter(&pixmap);

    int cols = maze.getCols(), rows = maze.getRows();
    double cellW = (double)pixmap.width()  / cols;
    double cellH = (double)pixmap.height() / rows;

    // Zbiory dla kolorowania
    std::set<std::pair<int,int>> visitedSet, solutionSet;
    if (res) {
        for (auto& p : res->visitedOrder) visitedSet.insert(p);
        for (auto& p : res->solution)     solutionSet.insert(p);
    }

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            QRectF rect(c * cellW, r * cellH, cellW, cellH);
            CellType ct = maze.getCell(c, r);
            std::pair<int,int> pos = {c, r};

            QColor color;
            if (solutionSet.count(pos))          color = QColor(255, 220, 0);
            else if (visitedSet.count(pos))       color = QColor(0, 120, 200);
            else if (ct == CellType::WALL)        color = QColor(40, 40, 50);
            else if (ct == CellType::START)       color = QColor(0, 220, 100);
            else if (ct == CellType::END)         color = QColor(220, 60, 60);
            else                                  color = QColor(220, 220, 230);

            painter.fillRect(rect, color);
        }
    }

    canvas->setPixmap(pixmap);
}

void AlgoVisualizer::on_mazeGenerateButton_clicked() {
    int size = ui->mazeSizeSelector->currentText().split("x").first().toInt();
    maze.generate(size, size);
    currentMazeResult = MazeResult{};
    renderMaze();
    ui->statusbar->showMessage(QString("Labirynt %1x%1 wygenerowany. Zielony=start, Czerwony=koniec.")
                                   .arg(size));
}

void AlgoVisualizer::on_mazeSolveButton_clicked() {
    if (maze.getCols() == 0) { ui->statusbar->showMessage("Najpierw wygeneruj labirynt."); return; }
    QString algo = ui->mazeSolverSelector->currentText();

    if      (algo == "BFS") currentMazeResult = maze.solveBFS();
    else if (algo == "DFS") currentMazeResult = maze.solveDFS();
    else if (algo == "A*")  currentMazeResult = maze.solveAStar();

    renderMaze(&currentMazeResult);
    if (currentMazeResult.found)
        ui->statusbar->showMessage(QString("%1 | Ścieżka: %2 kroków | Odwiedzone: %3 komórek")
                                       .arg(algo).arg(currentMazeResult.solution.size()).arg(currentMazeResult.visitedOrder.size()));
    else
        ui->statusbar->showMessage(algo + " | Nie znaleziono ścieżki!");
}
