#pragma once
#include <vector>
#include "WarShip.h"
#include "GameConfig.h"
#include "CoreTypes.h"

struct IWarShipGenerator
{
    virtual std::vector<WarShip> GenerateShips(const GameConfig& params) = 0;
};


class PredefinedWarShipGenerator : public IWarShipGenerator
{
    virtual std::vector<WarShip> GenerateShips(const GameConfig& params) override
    {
        // TODO DS extend mock version
        std::vector<CellIndex> cells = { {'a', 1}, {'a', 2} };
        WarShip ship_1(cells);
        return {ship_1};
    }
};

class WarShipGenerator : public IWarShipGenerator
{
    virtual std::vector<WarShip> GenerateShips(const GameConfig& params) override
    {
        // TODO AP real version
        return {};
    }
};
