#include "algovisualizer.h"
#include "./ui_algovisualizer.h"
#include "sorter.h"
#include <QPainter>
#include <QRandomGenerator>
#include <chrono>
#include <QThread>
#include <map>
#include <functional>
#include <cmath>

AlgoVisualizer::AlgoVisualizer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AlgoVisualizer)
{
    ui->setupUi(this);
    ui->resultLabel->setAlignment(Qt::AlignCenter);
    ui->resultLabel->setText("Wybierz parametry i naciśnij Sortuj");

    // Zakładka drzew - stan początkowy
    ui->treeCanvas->setAlignment(Qt::AlignCenter);
    ui->treeCanvas->setText("Drzewo jest puste. Wstaw węzeł.");
}

AlgoVisualizer::~AlgoVisualizer()
{
    delete ui;
}

// ============================================================
//  SORTING
// ============================================================

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
    int maxVal = 100;

    QFont font = painter.font();
    int fontSize = qBound(8, (int)(barWidth * 0.4), 12);
    font.setPixelSize(fontSize);
    painter.setFont(font);

    for (int i = 0; i < n; ++i) {
        int barHeight = (data[i] * (maxHeight - 20)) / maxVal;
        painter.setBrush(QColor(0, 180, 255));
        painter.setPen(QPen(Qt::black, 1));
        painter.drawRect(i * barWidth, maxHeight - barHeight, barWidth, barHeight);
        painter.setPen(Qt::white);
        QRect textRect(i * barWidth, maxHeight, barWidth, bottomSpace);
        painter.drawText(textRect, Qt::AlignCenter, QString::number(data[i]));
    }

    ui->resultLabel->setPixmap(pixmap);
    QCoreApplication::processEvents();
}

void AlgoVisualizer::on_sortButton_clicked()
{
    std::vector<int> data;
    int elementCount = 10;
    for(int i = 0; i < elementCount; ++i)
        data.push_back(QRandomGenerator::global()->bounded(5, 100));

    SortStats stats;
    bool shouldVisualize = ui->visualizeCheckBox->isChecked();
    AlgoVisualizer* vPtr = shouldVisualize ? this : nullptr;

    if (shouldVisualize) { renderArray(data); QThread::msleep(200); }
    else { ui->resultLabel->setPixmap(QPixmap()); ui->resultLabel->setText("Sortowanie w toku..."); }

    QString selected = ui->algoSelector->currentText();
    auto start = std::chrono::steady_clock::now();

    if      (selected == "Bubble Sort")    Sorter::bubbleSort(data, vPtr, stats);
    else if (selected == "Selection Sort") Sorter::selectionSort(data, vPtr, stats);
    else if (selected == "Insertion Sort") Sorter::insertionSort(data, vPtr, stats);
    else if (selected == "Quick Sort")     Sorter::quickSort(data, 0, data.size()-1, vPtr, stats);
    else if (selected == "Merge Sort")     Sorter::mergeSort(data, 0, data.size()-1, vPtr, stats);
    else if (selected == "Counting Sort")  Sorter::countingSort(data, vPtr, stats);
    else if (selected == "Bucket Sort")    Sorter::bucketSort(data, vPtr, stats);

    auto end = std::chrono::steady_clock::now();
    stats.durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    if (!shouldVisualize) renderArray(data);

    ui->statusbar->showMessage(
        QString("%1 | Czas: %2 ms | Porównania: %3 | Zamiany: %4")
            .arg(selected).arg(stats.durationMs).arg(stats.comparisons).arg(stats.swaps));
}

// ============================================================
//  TREE — helpers
// ============================================================

void AlgoVisualizer::updateTreeStats(const std::string& lastOp, int rotations,
                                     int comparisons, int nodeCount, int height)
{
    ui->statusbar->showMessage(
        QString("Operacja: %1 | Rotacje: %2 | Porównania: %3 | Węzły: %4 | Wysokość: %5")
            .arg(QString::fromStdString(lastOp))
            .arg(rotations).arg(comparisons).arg(nodeCount).arg(height));
}

// ============================================================
//  TREE — rendering
// ============================================================

void AlgoVisualizer::renderTree() {
    int idx = ui->treeTypeSelector->currentIndex(); // 0=AVL, 1=RB
    if (idx == 0) renderAVL();
    else          renderRB();
}

void AlgoVisualizer::renderAVL() {
    auto nodes = avlTree.getNodesForDrawing();

    if (nodes.empty()) {
        ui->treeCanvas->setPixmap(QPixmap());
        ui->treeCanvas->setText("Drzewo AVL jest puste.");
        return;
    }

    QPixmap pixmap(ui->treeCanvas->size());
    pixmap.fill(Qt::black);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = pixmap.width(), h = pixmap.height();
    int treeH = avlTree.getHeight();
    int levelH = std::max(40, h / (treeH + 1));
    int nodeR = qBound(12, levelH / 3, 22);

    // Oblicz pozycje X na podstawie pozycji w drzewie binarnym
    // Używamy pos w poziomie: dla głębokości d, mamy 2^d możliwych pozycji
    std::map<int, std::pair<int,int>> keyToPos; // key -> (cx, cy)

    for (auto& n : nodes) {
        int maxPosInLevel = (1 << n.depth); // 2^depth
        double slotW = (double)w / maxPosInLevel;
        int cx = (int)(slotW * n.pos + slotW / 2);
        int cy = levelH * n.depth + levelH / 2;
        cy = std::min(cy, h - nodeR - 5);
        keyToPos[n.key] = {cx, cy};
    }

    // Rysuj krawędzie
    painter.setPen(QPen(QColor(180, 180, 180), 1.5));
    for (auto& n : nodes) {
        if (n.hasParent && keyToPos.count(n.parentKey)) {
            auto [px, py] = keyToPos[n.parentKey];
            auto [cx, cy] = keyToPos[n.key];
            painter.drawLine(px, py, cx, cy);
        }
    }

    // Rysuj węzły
    QFont font = painter.font();
    font.setPixelSize(qBound(8, nodeR - 2, 13));
    font.setBold(true);
    painter.setFont(font);

    for (auto& n : nodes) {
        auto [cx, cy] = keyToPos[n.key];
        int bf = n.height; // tu height węzła
        // Kolor węzła w zależności od balance factor (poglądowo)
        painter.setBrush(QColor(0, 160, 220));
        painter.setPen(QPen(QColor(0, 220, 255), 2));
        painter.drawEllipse(cx - nodeR, cy - nodeR, nodeR*2, nodeR*2);

        painter.setPen(Qt::white);
        painter.drawText(QRect(cx - nodeR, cy - nodeR, nodeR*2, nodeR*2),
                         Qt::AlignCenter, QString::number(n.key));
    }

    ui->treeCanvas->setPixmap(pixmap);
}

void AlgoVisualizer::renderRB() {
    auto nodes = rbTree.getNodesForDrawing();

    if (nodes.empty()) {
        ui->treeCanvas->setPixmap(QPixmap());
        ui->treeCanvas->setText("Drzewo RB jest puste.");
        return;
    }

    QPixmap pixmap(ui->treeCanvas->size());
    pixmap.fill(Qt::black);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = pixmap.width(), h = pixmap.height();
    int treeH = rbTree.getHeight();
    int levelH = std::max(40, h / (treeH + 1));
    int nodeR = qBound(12, levelH / 3, 22);

    std::map<int, std::pair<int,int>> keyToPos;
    std::map<int, RBColor> keyToColor;

    for (auto& n : nodes) {
        int maxPosInLevel = (1 << n.depth);
        double slotW = (double)w / maxPosInLevel;
        int cx = (int)(slotW * n.pos + slotW / 2);
        int cy = levelH * n.depth + levelH / 2;
        cy = std::min(cy, h - nodeR - 5);
        keyToPos[n.key] = {cx, cy};
        keyToColor[n.key] = n.color;
    }

    // Krawędzie
    painter.setPen(QPen(QColor(160, 160, 160), 1.5));
    for (auto& n : nodes) {
        if (n.hasParent && keyToPos.count(n.parentKey)) {
            auto [px, py] = keyToPos[n.parentKey];
            auto [cx, cy] = keyToPos[n.key];
            painter.drawLine(px, py, cx, cy);
        }
    }

    // Węzły
    QFont font = painter.font();
    font.setPixelSize(qBound(8, nodeR - 2, 13));
    font.setBold(true);
    painter.setFont(font);

    for (auto& n : nodes) {
        auto [cx, cy] = keyToPos[n.key];
        QColor fillColor = (n.color == RED) ? QColor(220, 40, 40) : QColor(50, 50, 50);
        QColor borderColor = (n.color == RED) ? QColor(255, 100, 100) : QColor(140, 140, 140);

        painter.setBrush(fillColor);
        painter.setPen(QPen(borderColor, 2));
        painter.drawEllipse(cx - nodeR, cy - nodeR, nodeR*2, nodeR*2);

        painter.setPen(Qt::white);
        painter.drawText(QRect(cx - nodeR, cy - nodeR, nodeR*2, nodeR*2),
                         Qt::AlignCenter, QString::number(n.key));
    }

    ui->treeCanvas->setPixmap(pixmap);
}

// ============================================================
//  TREE — slots
// ============================================================

void AlgoVisualizer::on_treeTypeSelector_currentIndexChanged(int index) {
    Q_UNUSED(index);
    renderTree();
    ui->statusbar->clearMessage();
}

void AlgoVisualizer::on_treeInsertButton_clicked() {
    bool ok;
    int val = ui->treeValueInput->text().toInt(&ok);
    if (!ok) { ui->statusbar->showMessage("Błąd: podaj liczbę całkowitą."); return; }

    if (ui->treeTypeSelector->currentIndex() == 0) {
        avlTree.insert(val);
        updateTreeStats(avlTree.lastStats.lastOp, avlTree.lastStats.rotations,
                        avlTree.lastStats.comparisons, avlTree.getNodeCount(), avlTree.getHeight());
    } else {
        rbTree.insert(val);
        updateTreeStats(rbTree.lastStats.lastOp, rbTree.lastStats.rotations,
                        rbTree.lastStats.comparisons, rbTree.getNodeCount(), rbTree.getHeight());
    }
    renderTree();
}

void AlgoVisualizer::on_treeRemoveButton_clicked() {
    bool ok;
    int val = ui->treeValueInput->text().toInt(&ok);
    if (!ok) { ui->statusbar->showMessage("Błąd: podaj liczbę całkowitą."); return; }

    if (ui->treeTypeSelector->currentIndex() == 0) {
        avlTree.remove(val);
        updateTreeStats(avlTree.lastStats.lastOp, avlTree.lastStats.rotations,
                        avlTree.lastStats.comparisons, avlTree.getNodeCount(), avlTree.getHeight());
    } else {
        rbTree.remove(val);
        updateTreeStats(rbTree.lastStats.lastOp, rbTree.lastStats.rotations,
                        rbTree.lastStats.comparisons, rbTree.getNodeCount(), rbTree.getHeight());
    }
    renderTree();
}

void AlgoVisualizer::on_treeSearchButton_clicked() {
    bool ok;
    int val = ui->treeValueInput->text().toInt(&ok);
    if (!ok) { ui->statusbar->showMessage("Błąd: podaj liczbę całkowitą."); return; }

    bool found;
    if (ui->treeTypeSelector->currentIndex() == 0)
        found = avlTree.search(val);
    else
        found = rbTree.search(val);

    ui->statusbar->showMessage(found
                                   ? QString("Znaleziono: %1 ✓").arg(val)
                                   : QString("Nie znaleziono: %1 ✗").arg(val));
}

void AlgoVisualizer::on_treeClearButton_clicked() {
    if (ui->treeTypeSelector->currentIndex() == 0) avlTree.clear();
    else rbTree.clear();
    renderTree();
    ui->statusbar->showMessage("Drzewo wyczyszczone.");
}
