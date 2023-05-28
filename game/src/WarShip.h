#pragma once
#include "CellIndex.h"
#include <vector>

class WarShip
{
public:
    WarShip(const std::vector<CellIndex>& cells);

    [[nodiscard]] const std::vector<CellIndex> GetOccupiedCells() const;
    [[nodiscard]] int GetOccupiedCellsAmount() const; // aka ship type

    //bool IsDamaged() const; // optional
    [[nodiscard]] bool IsDestroyed() const;

    [[nodiscard]] bool DoesOccupyTheCell(const CellIndex& cell) const;
    void ShootShipAtCell(const CellIndex& cell);
    //bool TryShootShipAtCell(const CellIndex& cell); // optional

private:
    std::vector<std::pair<CellIndex, bool>> ShipDecks;
};