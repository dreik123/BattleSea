#pragma once
#include "Game/GameGrid.h"
#include "Game/WarShip.h"

#include <functional>

class CellIndex;

namespace GridUtilities
{
    void safeCellWalkthrough(const CellIndex& cell, std::function<void(int, int)> f);

    // !! Use this method only for ships positions without any other possible states like damaged or missed
    [[nodiscard]] const GameGrid convertShipsToGrid(const std::vector<WarShip>& ships);
}
