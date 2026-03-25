#include "algovisualizer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AlgoVisualizer w;
    w.show();
    return a.exec();
}
