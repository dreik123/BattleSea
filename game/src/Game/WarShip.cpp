#include "WarShip.h"
#include <algorithm>
#include <iterator>

WarShip::WarShip(const std::vector<CellIndex>& cells)
{
    for (const auto& cell : cells)
    {
        m_shipDecks.emplace_back(cell, true);
    }
}

const std::vector<CellIndex> WarShip::getOccupiedCells() const
{
    std::vector<CellIndex> cells;
    for (const auto& shipDeck : m_shipDecks)
    {
        std::transform(
            m_shipDecks.cbegin(),
            m_shipDecks.cend(),
            std::back_inserter(cells),
            [](const std::pair<CellIndex, bool>& p)
            {
                return p.first;
            });
    }
    return cells;
}

int WarShip::getOccupiedCellsAmount() const
{
    return static_cast<int>(m_shipDecks.size());
}

bool WarShip::isDestroyed() const
{
    const int count = std::count_if(
        m_shipDecks.cbegin(),
        m_shipDecks.cend(),
        [](const std::pair<CellIndex, bool>& p)
        {
            return p.second == true;
        });
    return count == 0;
}

bool WarShip::doesOccupyTheCell(const CellIndex& cell) const
{
    auto it = std::find_if(
        m_shipDecks.cbegin(),
        m_shipDecks.cend(),
        [&cell](const std::pair<CellIndex, bool>& p)
        {
            return p.first == cell;
        });
    return it != m_shipDecks.end();
}

void WarShip::shootShipAtCell(const CellIndex& cell)
{
    if (doesOccupyTheCell(cell))
    {
        auto it = std::find_if(
            m_shipDecks.begin(),
            m_shipDecks.end(),
            [&cell](const std::pair<CellIndex, bool>& p)
            {
                return p.first == cell;
            });
        it->second = false;
    }
}