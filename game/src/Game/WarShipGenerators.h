#pragma once

#include "WarShip.h"
#include "Core/CoreTypes.h"
#include "Core/BitMask.h"

#include <vector>
#include <random>

struct GameConfig;

enum class Direction
{
    North = 0,
    East,
    South,
    West,

    Count,
};

static constexpr int DIRECTION_COUNT = static_cast<int>(Direction::Count);
static_assert(DIRECTION_COUNT == 4);

using DirectionMask = BitMask<Direction>;

class IWarShipGenerator
{
public:
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
    WarShipGenerator();
    virtual const std::vector<WarShip> generateShips(const GameConfig& params) override;

private:
    bool setShipCell(
        const CellIndex& coord,
        const Direction& direction,
        DirectionMask& permissionDirections,
        const std::vector<std::vector<int>>& cells,
        std::vector<CellIndex>& shipCells
    );

    void fillAreaAroundShip(
        const std::vector<CellIndex>& shipCells,
        std::vector<std::vector<int>>& cells,
        const GameConfig& params
    );

private:
    std::random_device rd;
    std::mt19937 mt;
};
