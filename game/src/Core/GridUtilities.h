#pragma once
#include "CellIndex.h"
#include <functional>

void SafeCellWalkthrough(const CellIndex& cell, std::function<void(int, int)> f)
{
    // Example of class template auto deduction (CTAD)
    constexpr std::array POSSIBLE_CELL_DIRECTIONS =
    {
    std::make_pair(-1, 0), // up
    std::make_pair(-1, -1), // up-left
    std::make_pair(-1, 1), // up-right
    std::make_pair(0, -1), // left
    std::make_pair(0, 1), // right
    std::make_pair(1, 0), // down
    std::make_pair(1, -1), // down-left
    std::make_pair(1, 1), // down-right
    };

    const auto& [x, y] = cell.asIndexesPair();
    for (const auto& [dX, dY] : POSSIBLE_CELL_DIRECTIONS)
    {
        const int newX = x + dX;
        const int newY = y + dY;
        if (newX < 0 || newY < 0 || newX >= GridRowCount || newY >= GridColumnCount)
        {
            continue;
        }

        f(newX, newY);
    }
}