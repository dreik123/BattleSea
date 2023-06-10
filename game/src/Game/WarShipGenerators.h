#pragma once
#include <vector>

#include "WarShip.h"

#include "Core/CoreTypes.h"

struct GameConfig;

enum class Direction
{
    North = 0,
    East,
    South,
    West
};

struct IWarShipGenerator
{
    virtual const std::vector<WarShip> generateShips(const GameConfig& params) = 0;
};


class PredefinedClassicWarShipGenerator : public IWarShipGenerator
{
public:
    virtual const std::vector<WarShip> generateShips(const GameConfig&) override;
};


class WarShipGenerator : public IWarShipGenerator
{
public:
    virtual const std::vector<WarShip> generateShips(const GameConfig& params) override;
private:
    bool setShipCell(
        const CellIndex& coord,
        const Direction& direction,
        std::vector<Direction>& permissionDirections,
        const std::vector<std::vector<int>>& cells,
        std::vector<CellIndex>& shipCells);
    void fillAreaAroundShip(const std::vector<CellIndex>& shipCells, std::vector<std::vector<int>>& cells, const GameConfig& params);
};
