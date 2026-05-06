#ifndef SORTER_H
#define SORTER_H

#include <vector>

class AlgoVisualizer;

struct SortStats {
    long long durationMs = 0;
    int comparisons = 0;
    int swaps = 0;
};

class Sorter {
public:
    static void bubbleSort(std::vector<int>& data, AlgoVisualizer* v, SortStats& s);
    static void selectionSort(std::vector<int>& data, AlgoVisualizer* v, SortStats& s);
    static void insertionSort(std::vector<int>& data, AlgoVisualizer* v, SortStats& s);
    static void quickSort(std::vector<int>& data, int low, int high, AlgoVisualizer* v, SortStats& s);
    static void mergeSort(std::vector<int>& data, int l, int r, AlgoVisualizer* v, SortStats& s);
    static void countingSort(std::vector<int>& data, AlgoVisualizer* v, SortStats& s);
    static void bucketSort(std::vector<int>& data, AlgoVisualizer* v, SortStats& s);

private:
    static void merge(std::vector<int>& data, int l, int m, int r, AlgoVisualizer* v, SortStats& s);
    static int partition(std::vector<int>& data, int low, int high, AlgoVisualizer* v, SortStats& s);
};

#endif // SORTER_H
