#include "algovisualizer.h"
#include "./ui_algovisualizer.h"
#include "sorter.h"
#include <vector>
#include <random>

AlgoVisualizer::AlgoVisualizer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AlgoVisualizer)
{
    ui->setupUi(this);
    ui->resultLabel->setText("Gotowy do sortowania...");
    connect(ui->sortButton, &QPushButton::clicked, this, &AlgoVisualizer::on_sortButton_clicked);
}

AlgoVisualizer::~AlgoVisualizer()
{
    delete ui;
}

void AlgoVisualizer::on_sortButton_clicked()
{
    // 1. Generator liczb losowych
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 100); // Przedział 0-100

    // 2. Tworzenie wektora z 10 losowymi liczbami
    std::vector<int> data;
    for(int i = 0; i < 10; ++i) {
        data.push_back(dis(gen));
    }

    // 3. Przygotowanie tekstu "Przed"
    QString resultText = "Przed: ";
    for (int n : data) resultText += QString::number(n) + " ";
    resultText += "\n";

    // 4. Pobranie wybranego algorytmu i sortowanie (Etap 2)
    QString selected = ui->algoSelector->currentText();
    if (selected == "Bubble Sort") {
        Sorter::bubbleSort(data);
    } else {
        Sorter::selectionSort(data);
    }

    // 5. Przygotowanie tekstu "Po"
    resultText += "Po:    ";
    for (int n : data) resultText += QString::number(n) + " ";

    // 6. Wyświetlenie w Labelu
    ui->resultLabel->setText(resultText);
}
