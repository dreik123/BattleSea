#include "WarShip.h"
#include <algorithm>

WarShip::WarShip(const std::vector<CellIndex>& cells)
{
    for (auto& cell : cells)
    {
        ShipDecks.push_back(std::pair<CellIndex, bool>(cell, true));
    }
}

std::vector<CellIndex> WarShip::GetOccupiedCells() const
{
    std::vector<CellIndex> cells;
    for(auto& shipdeck : ShipDecks)
    {
        cells.push_back(shipdeck.first);
    }
    return cells;
}

int WarShip::GetOccupiedCellsAmount() const 
{
    return (int)ShipDecks.size();
}

bool WarShip::IsDestroyed() const 
{
    if (auto count = std::count_if(ShipDecks.begin(), ShipDecks.end(), 
                                   [](std::pair<CellIndex, bool> p){ return p.second == true; }); 
        count == 0)
    {
        return true;
    }
    return false; 
}

bool WarShip::DoesOccupyTheCell(const CellIndex& cell) const 
{
    if (auto it = std::find_if(ShipDecks.begin(), ShipDecks.end(), 
                               [cell](std::pair<CellIndex, bool> p) {return p.first.ToString() == cell.ToString(); });
        it != ShipDecks.end())
    {
        return true;
    }

    return false; 
}

void WarShip::ShootShipAtCell(const CellIndex& cell) 
{
    if (DoesOccupyTheCell(cell))
    {
        auto it = std::find_if(ShipDecks.begin(), ShipDecks.end(), 
                               [cell](std::pair<CellIndex, bool> p) {return p.first.ToString() == cell.ToString(); });
        it->second = false;
    }
}