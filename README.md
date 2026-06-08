# AlgoVisualizer

Interaktywna aplikacja desktopowa do wizualizacji algorytmów i struktur danych w czasie rzeczywistym.

## 🎯 Cel projektu

Projekt powstał jako narzędzie edukacyjne pozwalające na graficzne śledzenie działania algorytmów krok po kroku oraz porównywanie ich wydajności na podstawie mierzonych statystyk.

---

## 🚀 Funkcjonalności

### 📊 Algorytmy sortowania
Wizualizacja działania algorytmu w czasie rzeczywistym — każda zamiana i porównanie jest animowane na wykresie słupkowym. Po zakończeniu wyświetlane są statystyki: czas wykonania, liczba porównań i liczba zamian.
- Bubble Sort, Insertion Sort, Selection Sort
- Quick Sort, Merge Sort
- Counting Sort, Bucket Sort

### 🌳 Struktury danych
Interaktywny edytor drzew z wizualizacją węzłów i krawędzi. Po każdej operacji status bar wyświetla liczbę rotacji i porównań.
- **Drzewo AVL** — automatyczne równoważenie po każdej operacji (rotacje LL, LR, RL, RR)
- **Drzewo czerwono-czarne** — kolorowanie węzłów i fixup po wstawieniu i usunięciu

### 🔗 Grafy
Generator losowych grafów z gwarantowaną osiągalnością każdego węzła — zarówno dla grafów skierowanych jak i nieskierowanych. Odwiedzone węzły i krawędzie drzewa wynikowego są podświetlane w czasie rzeczywistym.
- BFS — przeszukiwanie wszerz
- DFS — przeszukiwanie w głąb
- Dijkstra — najkrótsza ścieżka

### 🧩 Labirynt
Generator losowych labiryntów oparty na algorytmie Recursive Backtracker — każdy labirynt jest unikalny i w pełni spójny. Wizualizacja pokazuje odwiedzone komórki i znalezioną ścieżkę.
- BFS — gwarantuje najkrótszą ścieżkę
- DFS — szybki, eksploruje głęboko

---

## 🛠 Technologie

- **Język:** C++17
- **UI:** Qt 6 (Qt Widgets, QPainter)
- **Build system:** CMake
- **Kontrola wersji:** GitHub

---

## 👤 Autor

Filip Koczorowski
