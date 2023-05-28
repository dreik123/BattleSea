#include "WarShip.h"
#include <algorithm>
#include <iterator>

WarShip::WarShip(const std::vector<CellIndex>& cells)
{
    for (const auto& cell : cells)
    {
        ShipDecks.emplace_back(cell, true);
    }
}

const std::vector<CellIndex> WarShip::GetOccupiedCells() const
{
    std::vector<CellIndex> cells;
    for(const auto& shipdeck : ShipDecks)
    {
        std::transform(ShipDecks.cbegin(), ShipDecks.cend(), std::back_inserter(cells),
                       [](const std::pair<CellIndex, bool>& p) { return p.first; });
    }
    return cells;
}

int WarShip::GetOccupiedCellsAmount() const 
{
    return static_cast<int>(ShipDecks.size());
}

bool WarShip::IsDestroyed() const 
{
    const int count = std::count_if(ShipDecks.cbegin(), ShipDecks.cend(), 
                                    [](const std::pair<CellIndex, bool>& p){ return p.second == true; }); 
    return count == 0;
}

bool WarShip::DoesOccupyTheCell(const CellIndex& cell) const 
{
    auto it = std::find_if(ShipDecks.cbegin(), ShipDecks.cend(), 
                           [&cell](const std::pair<CellIndex, bool>& p) { return p.first == cell; });
    return it != ShipDecks.end();
}

void WarShip::ShootShipAtCell(const CellIndex& cell) 
{
    if (DoesOccupyTheCell(cell))
    {
        auto it = std::find_if(ShipDecks.begin(), ShipDecks.end(), 
                               [&cell](const std::pair<CellIndex, bool>& p) { return p.first == cell; });
        it->second = false;
    }
}