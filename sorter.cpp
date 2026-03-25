#ifndef SORTER_H
#define SORTER_H

#include <vector>
#include <algorithm>

class Sorter {
public:
    // Implementacja Bubble Sort (wymagany w Etapie 2)
    static void bubbleSort(std::vector<int>& data) {
        int n = data.size();
        for (int i = 0; i < n - 1; i++) {
            for (int j = 0; j < n - i - 1; j++) {
                if (data[j] > data[j + 1]) {
                    std::swap(data[j], data[j + 1]);
                }
            }
        }
    }

    // Implementacja Selection Sort (wymagany w Etapie 2)
    static void selectionSort(std::vector<int>& data) {
        int n = data.size();
        for (int i = 0; i < n - 1; i++) {
            int min_idx = i;
            for (int j = i + 1; j < n; j++)
                if (data[j] < data[min_idx]) min_idx = j;
            std::swap(data[min_idx], data[i]);
        }
    }
};

#endif
