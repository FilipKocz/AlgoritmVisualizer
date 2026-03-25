#ifndef SORTER_H
#define SORTER_H

#include <vector>
#include <algorithm>

class Sorter {
public:
    static void bubbleSort(std::vector<int>& data) {
        for (size_t i = 0; i < data.size(); i++)
            for (size_t j = 0; j < data.size() - 1; j++)
                if (data[j] > data[j+1]) std::swap(data[j], data[j+1]);
    }

    static void selectionSort(std::vector<int>& data) {
        for (size_t i = 0; i < data.size(); i++) {
            size_t minIdx = i;
            for (size_t j = i + 1; j < data.size(); j++)
                if (data[j] < data[minIdx]) minIdx = j;
            std::swap(data[i], data[minIdx]);
        }
    }
};

#endif
