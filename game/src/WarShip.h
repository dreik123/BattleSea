#pragma once
#include "CellIndex.h"
#include <vector>

class WarShip
{
public:
    WarShip(const std::vector<CellIndex>& cells);

    std::vector<CellIndex> GetOccupiedCells() const;
    int GetOccupiedCellsAmount() const; // aka ship type

    //bool IsDamaged() const; // optional
    bool IsDestroyed() const;

    bool DoesOccupyTheCell(const CellIndex& cell) const;
    void ShootShipAtCell(const CellIndex& cell);
    //bool TryShootShipAtCell(const CellIndex& cell); // optional

private:
    std::vector<std::pair<CellIndex, bool>> ShipDecks;
};