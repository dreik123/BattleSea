#pragma once
#include "CellIndex.h"

// TODO AP impl this
class WarShip
{
public:
    WarShip(const std::vector<CellIndex>& cells) {}
    
public:
    std::vector<CellIndex> GetOccupiedCells() const { return {}; }
    int GetOccupiedCellsAmount() const { return 0; } // aka ship type

    //bool IsDamaged() const; // optional
    bool IsDestroyed() const { return false; } // TODO std algorithm

    bool DoesOccupyTheCell(const CellIndex& cell) const { return false; }
    void ShootShipAtCell(const CellIndex& cell) {}
    //bool TryShootShipAtCell(const CellIndex& cell); // optional

private:

};