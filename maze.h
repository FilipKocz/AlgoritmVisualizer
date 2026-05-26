#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <utility>

enum class CellType { WALL, PATH, START, END, VISITED, SOLUTION };

struct MazeCell {
    CellType type = CellType::WALL;
};

struct MazeResult {
    std::vector<std::pair<int,int>> visitedOrder;  // kolejność odwiedzonych komórek
    std::vector<std::pair<int,int>> solution;       // ścieżka rozwiązania
    int steps = 0;
    bool found = false;
};

class Maze {
public:
    Maze();

    void generate(int cols, int rows);  // generowanie labiryntu (DFS/Recursive Backtracker)
    void setStart(int col, int row);
    void setEnd(int col, int row);

    MazeResult solveBFS();
    MazeResult solveDFS();
    MazeResult solveAStar();

    int getCols() const { return cols; }
    int getRows() const { return rows; }
    CellType getCell(int col, int row) const;
    std::pair<int,int> getStart() const { return start; }
    std::pair<int,int> getEnd() const { return end; }

    const std::vector<std::vector<MazeCell>>& getGrid() const { return grid; }

private:
    int cols, rows;
    std::vector<std::vector<MazeCell>> grid;
    std::pair<int,int> start, end;

    void carvePath(int col, int row);  // rekurencyjny DFS do generowania
    bool inBounds(int col, int row) const;
};

#endif // MAZE_H
