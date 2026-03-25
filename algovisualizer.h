#ifndef ALGOVISUALIZER_H
#define ALGOVISUALIZER_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class AlgoVisualizer;
}
QT_END_NAMESPACE

class AlgoVisualizer : public QMainWindow
{
    Q_OBJECT

public:
    AlgoVisualizer(QWidget *parent = nullptr);
    ~AlgoVisualizer();

private:
    Ui::AlgoVisualizer *ui;
    void on_sortButton_clicked();
};
#endif // ALGOVISUALIZER_H
