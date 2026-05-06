#ifndef ALGOVISUALIZER_H
#define ALGOVISUALIZER_H

#include <QMainWindow>
#include <vector>

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

private:
    Ui::AlgoVisualizer *ui;
};
#endif
