#include "maze.h"
#include <algorithm>
#include <random>
#include <queue>
#include <stack>
#include <map>
#include <cmath>
#include <functional>

Maze::Maze() : cols(0), rows(0), start({1,1}), end({1,1}) {}

bool Maze::inBounds(int col, int row) const {
    return col >= 0 && col < cols && row >= 0 && row < rows;
}

CellType Maze::getCell(int col, int row) const {
    if (!inBounds(col, row)) return CellType::WALL;
    return grid[row][col].type;
}

// ============================================================
//  GENEROWANIE — Recursive Backtracker (DFS)
//  Labirynt jest generowany na siatce nieparzystych komórek.
//  Każda komórka PATH jest oddzielona ścianami szerokości 1.
// ============================================================
void Maze::generate(int c, int r) {
    // Wymiary muszą być nieparzyste żeby algorytm działał poprawnie
    cols = (c % 2 == 0) ? c + 1 : c;
    rows = (r % 2 == 0) ? r + 1 : r;

    // Wypełnij wszystko ścianami
    grid.assign(rows, std::vector<MazeCell>(cols));

    // Wydrąż ścieżki rekurencyjnym DFS
    carvePath(1, 1);

    // Ustaw start i koniec
    start = {1, 1};
    end   = {cols - 2, rows - 2};
    grid[start.second][start.first].type = CellType::START;
    grid[end.second][end.first].type     = CellType::END;
}

void Maze::carvePath(int col, int row) {
    static std::mt19937 rng(std::random_device{}());

    grid[row][col].type = CellType::PATH;

    // 4 kierunki: prawo, lewo, dół, góra (krok o 2)
    std::vector<std::pair<int,int>> dirs = {{2,0},{-2,0},{0,2},{0,-2}};
    std::shuffle(dirs.begin(), dirs.end(), rng);

    for (auto [dc, dr] : dirs) {
        int nc = col + dc;
        int nr = row + dr;
        if (inBounds(nc, nr) && grid[nr][nc].type == CellType::WALL) {
            // Usuń ścianę pomiędzy
            grid[row + dr/2][col + dc/2].type = CellType::PATH;
            carvePath(nc, nr);
        }
    }
}

void Maze::setStart(int col, int row) {
    if (!inBounds(col, row)) return;
    if (grid[start.second][start.first].type == CellType::START)
        grid[start.second][start.first].type = CellType::PATH;
    start = {col, row};
    grid[row][col].type = CellType::START;
}

void Maze::setEnd(int col, int row) {
    if (!inBounds(col, row)) return;
    if (grid[end.second][end.first].type == CellType::END)
        grid[end.second][end.first].type = CellType::PATH;
    end = {col, row};
    grid[row][col].type = CellType::END;
}

// ============================================================
//  BFS
// ============================================================
MazeResult Maze::solveBFS() {
    MazeResult result;
    auto [sc, sr] = start;
    auto [ec, er] = end;

    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    std::map<std::pair<int,int>, std::pair<int,int>> parent;
    std::queue<std::pair<int,int>> q;

    visited[sr][sc] = true;
    q.push({sc, sr});
    parent[{sc,sr}] = {-1,-1};

    std::vector<std::pair<int,int>> dirs = {{1,0},{-1,0},{0,1},{0,-1}};

    while (!q.empty()) {
        auto [cc, cr] = q.front(); q.pop();
        result.visitedOrder.push_back({cc, cr});
        result.steps++;

        if (cc == ec && cr == er) { result.found = true; break; }

        for (auto [dc, dr] : dirs) {
            int nc = cc + dc, nr = cr + dr;
            if (inBounds(nc, nr) && !visited[nr][nc] &&
                grid[nr][nc].type != CellType::WALL) {
                visited[nr][nc] = true;
                parent[{nc,nr}] = {cc,cr};
                q.push({nc, nr});
            }
        }
    }

    // Odtwórz ścieżkę
    if (result.found) {
        std::pair<int,int> cur = {ec, er};
        while (cur.first != -1) {
            result.solution.push_back(cur);
            cur = parent[cur];
        }
        std::reverse(result.solution.begin(), result.solution.end());
    }
    return result;
}

// ============================================================
//  DFS
// ============================================================
MazeResult Maze::solveDFS() {
    MazeResult result;
    auto [sc, sr] = start;
    auto [ec, er] = end;

    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    std::map<std::pair<int,int>, std::pair<int,int>> parent;

    std::function<bool(int,int)> dfs = [&](int cc, int cr) -> bool {
        visited[cr][cc] = true;
        result.visitedOrder.push_back({cc, cr});
        result.steps++;

        if (cc == ec && cr == er) return true;

        std::vector<std::pair<int,int>> dirs = {{1,0},{0,1},{-1,0},{0,-1}};
        for (auto [dc, dr] : dirs) {
            int nc = cc + dc, nr = cr + dr;
            if (inBounds(nc, nr) && !visited[nr][nc] &&
                grid[nr][nc].type != CellType::WALL) {
                parent[{nc,nr}] = {cc,cr};
                if (dfs(nc, nr)) return true;
            }
        }
        return false;
    };

    parent[{sc,sr}] = {-1,-1};
    result.found = dfs(sc, sr);

    if (result.found) {
        std::pair<int,int> cur = {ec, er};
        while (cur.first != -1) {
            result.solution.push_back(cur);
            cur = parent[cur];
        }
        std::reverse(result.solution.begin(), result.solution.end());
    }
    return result;
}

// ============================================================
//  A*
// ============================================================
MazeResult Maze::solveAStar() {
    MazeResult result;
    auto [sc, sr] = start;
    auto [ec, er] = end;

    // Heurystyka: dystans Manhattan
    auto heuristic = [&](int c, int r) {
        return std::abs(c - ec) + std::abs(r - er);
    };

    std::map<std::pair<int,int>, int> gScore, fScore;
    std::map<std::pair<int,int>, std::pair<int,int>> parent;
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));

    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++) {
            gScore[{c,r}] = INT_MAX;
            fScore[{c,r}] = INT_MAX;
        }

    gScore[{sc,sr}] = 0;
    fScore[{sc,sr}] = heuristic(sc, sr);
    parent[{sc,sr}] = {-1,-1};

    // min-heap po fScore
    using T = std::tuple<int, int, int>; // fScore, col, row
    std::priority_queue<T, std::vector<T>, std::greater<T>> open;
    open.push({fScore[{sc,sr}], sc, sr});

    std::vector<std::pair<int,int>> dirs = {{1,0},{-1,0},{0,1},{0,-1}};

    while (!open.empty()) {
        auto [f, cc, cr] = open.top(); open.pop();
        if (visited[cr][cc]) continue;
        visited[cr][cc] = true;
        result.visitedOrder.push_back({cc, cr});
        result.steps++;

        if (cc == ec && cr == er) { result.found = true; break; }

        for (auto [dc, dr] : dirs) {
            int nc = cc + dc, nr = cr + dr;
            if (!inBounds(nc, nr) || visited[nr][nc] ||
                grid[nr][nc].type == CellType::WALL) continue;

            int tentG = gScore[{cc,cr}] + 1;
            if (tentG < gScore[{nc,nr}]) {
                parent[{nc,nr}] = {cc,cr};
                gScore[{nc,nr}] = tentG;
                fScore[{nc,nr}] = tentG + heuristic(nc, nr);
                open.push({fScore[{nc,nr}], nc, nr});
            }
        }
    }

    if (result.found) {
        std::pair<int,int> cur = {ec, er};
        while (cur.first != -1) {
            result.solution.push_back(cur);
            cur = parent[cur];
        }
        std::reverse(result.solution.begin(), result.solution.end());
    }
    return result;
}
