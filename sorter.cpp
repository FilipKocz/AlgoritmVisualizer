#include "sorter.h"
#include "algovisualizer.h"
#include <algorithm>
#include <QThread>

const int SLEEP_TIME = 300; // Czas przerwy dla animacji

void Sorter::bubbleSort(std::vector<int>& data, AlgoVisualizer* v, SortStats& s) {
    int n = data.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            s.comparisons++;
            if (data[j] > data[j + 1]) {
                std::swap(data[j], data[j + 1]);
                s.swaps++;
                if (v) {
                    v->renderArray(data);
                    QThread::msleep(SLEEP_TIME);
                }
            }
        }
    }
}

void Sorter::selectionSort(std::vector<int>& data, AlgoVisualizer* v, SortStats& s) {
    int n = data.size();
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            s.comparisons++;
            if (data[j] < data[min_idx]) min_idx = j;
        }
        std::swap(data[min_idx], data[i]);
        s.swaps++;
        if (v) {
            v->renderArray(data);
            QThread::msleep(SLEEP_TIME + 20);
        }
    }
}

void Sorter::insertionSort(std::vector<int>& data, AlgoVisualizer* v, SortStats& s) {
    int n = data.size();
    for (int i = 1; i < n; i++) {
        int key = data[i], j = i - 1;
        while (j >= 0 && data[j] > key) {
            s.comparisons++;
            data[j + 1] = data[j];
            s.swaps++;
            j--;
            if (v) {
                v->renderArray(data);
                QThread::msleep(SLEEP_TIME / 2);
            }
        }
        data[j + 1] = key;
        if (v) v->renderArray(data);
    }
}

int Sorter::partition(std::vector<int>& data, int low, int high, AlgoVisualizer* v, SortStats& s) {
    int pivot = data[high], i = (low - 1);
    for (int j = low; j < high; j++) {
        s.comparisons++;
        if (data[j] < pivot) {
            i++;
            std::swap(data[i], data[j]);
            s.swaps++;
            if (v) {
                v->renderArray(data);
                QThread::msleep(SLEEP_TIME);
            }
        }
    }
    std::swap(data[i + 1], data[high]);
    s.swaps++;
    if (v) v->renderArray(data);
    return (i + 1);
}

void Sorter::quickSort(std::vector<int>& data, int low, int high, AlgoVisualizer* v, SortStats& s) {
    if (low < high) {
        int pi = partition(data, low, high, v, s);
        quickSort(data, low, pi - 1, v, s);
        quickSort(data, pi + 1, high, v, s);
    }
}

void Sorter::merge(std::vector<int>& data, int l, int m, int r, AlgoVisualizer* v, SortStats& s) {
    int n1 = m - l + 1, n2 = r - m;
    std::vector<int> L(n1), R(n2);
    for (int i = 0; i < n1; i++) L[i] = data[l + i];
    for (int j = 0; j < n2; j++) R[j] = data[m + 1 + j];
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        s.comparisons++;
        if (L[i] <= R[j]) data[k] = L[i++];
        else data[k] = R[j++];
        s.swaps++;
        k++;
        if (v) { v->renderArray(data); QThread::msleep(SLEEP_TIME); }
    }
    while (i < n1) { data[k++] = L[i++]; if (v) { v->renderArray(data); QThread::msleep(SLEEP_TIME); } }
    while (j < n2) { data[k++] = R[j++]; if (v) { v->renderArray(data); QThread::msleep(SLEEP_TIME); } }
}

void Sorter::mergeSort(std::vector<int>& data, int l, int r, AlgoVisualizer* v, SortStats& s) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(data, l, m, v, s);
        mergeSort(data, m + 1, r, v, s);
        merge(data, l, m, r, v, s);
    }
}

void Sorter::countingSort(std::vector<int>& data, AlgoVisualizer* v, SortStats& s) {
    if (data.empty()) return;
    int max = *std::max_element(data.begin(), data.end());
    std::vector<int> count(max + 1, 0);
    for (int x : data) count[x]++;
    int index = 0;
    for (int i = 0; i <= max; i++) {
        while (count[i]-- > 0) {
            data[index++] = i;
            s.swaps++;
            if (v) {
                v->renderArray(data);
                QThread::msleep(SLEEP_TIME);
            }
        }
    }
}

void Sorter::bucketSort(std::vector<int>& data, AlgoVisualizer* v, SortStats& s) {
    if (data.empty()) return;
    int n = data.size(), max_val = *std::max_element(data.begin(), data.end());
    std::vector<std::vector<int>> buckets(n);
    for (int x : data) {
        int bi = (n - 1) * x / (max_val == 0 ? 1 : max_val);
        buckets[bi].push_back(x);
    }
    int index = 0;
    for (int i = 0; i < n; i++) {
        std::sort(buckets[i].begin(), buckets[i].end());
        for (int x : buckets[i]) {
            data[index++] = x;
            s.swaps++;
            if (v) {
                v->renderArray(data);
                QThread::msleep(SLEEP_TIME);
            }
        }
    }
}
