#pragma once
#include <vector>
#include <array>

#include "WarShip.h"
#include "GameConfig.h"
#include "CoreTypes.h"

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
