#pragma once
#include "Core/CellIndex.h"
#include <vector>

class WarShip
{
public:
    WarShip(const std::vector<CellIndex>& cells);

    [[nodiscard]] const std::vector<CellIndex> getOccupiedCells() const;
    [[nodiscard]] int getOccupiedCellsAmount() const; // aka ship type

    //bool IsDamaged() const; // optional
    [[nodiscard]] bool isDestroyed() const;

    [[nodiscard]] bool doesOccupyTheCell(const CellIndex& cell) const;
    void shootShipAtCell(const CellIndex& cell);
    //bool TryShootShipAtCell(const CellIndex& cell); // optional

private:
    std::vector<std::pair<CellIndex, bool>> m_shipDecks;
};