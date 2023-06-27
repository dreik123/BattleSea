#pragma once

#include "Core/CoreTypes.h"
#include "Core/CellIndex.h"
#include "GameConfig.h"

// README if we decide to customize grid size based on config, need to consider std::vector instead of static array
template<int Rows, int Columns>
struct BaseGrid
{
    BaseGrid()
    {
        for (auto& gridRow : data)
        {
            for (CellState& cell : gridRow)
            {
                cell = CellState::Concealed;
            }
        }
    }

    [[nodiscard]] CellState at(const CellIndex& cell) const { return data[cell.x()][cell.y()]; }
    [[nodiscard]] CellState& at(const CellIndex& cell) { return data[cell.x()][cell.y()]; }

    std::array<CellState, CLASSIC_GRID_COLUMN_COUNT>& operator[] (const int index) { return data[index]; }
    const std::array<CellState, CLASSIC_GRID_COLUMN_COUNT>& operator[] (const int index) const { return data[index]; }

public:
    std::array<std::array<CellState, Columns>, Rows> data;
};


using GameGrid = BaseGrid<CLASSIC_GRID_ROW_COUNT, CLASSIC_GRID_COLUMN_COUNT>;
