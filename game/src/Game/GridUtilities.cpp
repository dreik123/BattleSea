#include "GridUtilities.h"

#include "Core/CellIndex.h"

namespace GridUtilities
{
    void safeCellWalkthrough(const CellIndex& cell, std::function<void(int, int)> f)
    {
        // Example of class template auto deduction (CTAD)
        constexpr std::array possibleCellDirections =
        {
            std::make_pair(-1, 0),  // up
            std::make_pair(-1, -1), // up-left
            std::make_pair(-1, 1),  // up-right
            std::make_pair(0, -1),  // left
            std::make_pair(0, 1),   // right
            std::make_pair(1, 0),   // down
            std::make_pair(1, -1),  // down-left
            std::make_pair(1, 1),   // down-right
        };

        const auto& [x, y] = cell.asIndexesPair();
        for (const auto& [dX, dY] : possibleCellDirections)
        {
            const int newX = x + dX;
            const int newY = y + dY;
            if (newX < 0 || newY < 0 || newX >= CLASSIC_GRID_ROW_COUNT || newY >= CLASSIC_GRID_COLUMN_COUNT)
            {
                continue;
            }

            f(newX, newY);
        }
    }

    // !! Use this method only for ships positions without any other possible states like damaged or missed
    const GameGrid convertShipsToGrid(const std::vector<WarShip>& ships)
    {
        GameGrid grid; // empty grid with everything concealed

        for (const WarShip& ship : ships)
        {
            for (const CellIndex& cell : ship.getOccupiedCells())
            {
                grid.at(cell) = CellState::Ship;
            }
        }

        return grid;
    }
}
