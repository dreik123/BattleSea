#pragma once
#include <vector>

#include "Game/WarShip.h"
#include "Game/GameConfig.h"
#include "Core/CoreTypes.h"

struct IWarShipGenerator
{
    virtual const std::vector<WarShip> GenerateShips(const GameConfig& params) = 0;
};


class PredefinedClassicWarShipGenerator : public IWarShipGenerator
{
public:
    virtual const std::vector<WarShip> GenerateShips(const GameConfig&) override;
};


class WarShipGenerator : public IWarShipGenerator
{
public:
    virtual const std::vector<WarShip> GenerateShips(const GameConfig& params) override;
};
