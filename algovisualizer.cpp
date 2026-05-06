#include "algovisualizer.h"
#include "./ui_algovisualizer.h"
#include "sorter.h"
#include <QPainter>
#include <QRandomGenerator>
#include <chrono>
#include <QThread>

AlgoVisualizer::AlgoVisualizer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AlgoVisualizer)
{
    ui->setupUi(this);
    ui->resultLabel->setAlignment(Qt::AlignCenter);
    ui->resultLabel->setText("Wybierz parametry i naciśnij Sortuj");
}

AlgoVisualizer::~AlgoVisualizer()
{
    delete ui;
}

void AlgoVisualizer::renderArray(const std::vector<int>& data) {
    if (ui->resultLabel->width() <= 0 || ui->resultLabel->height() <= 0) return;

    QPixmap pixmap(ui->resultLabel->size());
    pixmap.fill(Qt::black);

    QPainter painter(&pixmap);
    int n = data.size();
    if (n == 0) return;

    // --- LOGIKA RYSOWANIA LICZB ---
    int bottomSpace = 25;
    double barWidth = (double)pixmap.width() / n;
    int maxHeight = pixmap.height() - bottomSpace;
    int maxVal = 100;

    //wielkości czcionki w zależności od szerokości słupka
    QFont font = painter.font();
    int fontSize = qBound(8, (int)(barWidth * 0.4), 12);
    font.setPixelSize(fontSize);
    painter.setFont(font);

    for (int i = 0; i < n; ++i) {
        // 1. Rysowanie słupka
        int barHeight = (data[i] * (maxHeight - 20)) / maxVal;

        painter.setBrush(QColor(0, 180, 255));
        painter.setPen(QPen(Qt::black, 1));
        painter.drawRect(i * barWidth, maxHeight - barHeight, barWidth, barHeight);

        // 2. Rysowanie liczby pod słupkiem
        painter.setPen(Qt::white);
        QString valStr = QString::number(data[i]);

        QRect textRect(i * barWidth, maxHeight, barWidth, bottomSpace);
        painter.drawText(textRect, Qt::AlignCenter, valStr);
    }

    ui->resultLabel->setPixmap(pixmap);
    QCoreApplication::processEvents();
}

void AlgoVisualizer::on_sortButton_clicked()
{
    // 1. Generowanie danych
    std::vector<int> data;
    int elementCount = 10;
    for(int i = 0; i < elementCount; ++i) {
        data.push_back(QRandomGenerator::global()->bounded(5, 100));
    }

    SortStats stats;
    bool shouldVisualize = ui->visualizeCheckBox->isChecked();

    AlgoVisualizer* vPtr = shouldVisualize ? this : nullptr;

    if (shouldVisualize) {
        renderArray(data);
        QThread::msleep(200);
    } else {
        ui->resultLabel->setPixmap(QPixmap());
        ui->resultLabel->setText("Sortowanie w toku (bez wizualizacji)...");
    }

    QString selected = ui->algoSelector->currentText();

    // 3. Start pomiaru i algorytmu
    auto start = std::chrono::steady_clock::now();

    if (selected == "Bubble Sort") {
        Sorter::bubbleSort(data, vPtr, stats);
    } else if (selected == "Selection Sort") {
        Sorter::selectionSort(data, vPtr, stats);
    } else if (selected == "Insertion Sort") {
        Sorter::insertionSort(data, vPtr, stats);
    } else if (selected == "Quick Sort") {
        Sorter::quickSort(data, 0, data.size() - 1, vPtr, stats);
    } else if (selected == "Merge Sort") {
        Sorter::mergeSort(data, 0, data.size() - 1, vPtr, stats);
    } else if (selected == "Counting Sort") {
        Sorter::countingSort(data, vPtr, stats);
    } else if (selected == "Bucket Sort") {
        Sorter::bucketSort(data, vPtr, stats);
    }

    auto end = std::chrono::steady_clock::now();
    stats.durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // 4. Wyświetlenie końcowe
    if (!shouldVisualize) renderArray(data);

    QString resultMsg = QString("%1 | Czas: %2 ms | Porównania: %3 | Zamiany: %4")
                            .arg(selected)
                            .arg(stats.durationMs)
                            .arg(stats.comparisons)
                            .arg(stats.swaps);

    ui->statusbar->showMessage(resultMsg);
}
