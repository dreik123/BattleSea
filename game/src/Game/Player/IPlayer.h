#pragma once

#include <string>
#include <optional>

#include "Core/CoreTypes.h"

struct InputRequest
{
    std::optional<CellIndex> shotCell;
    bool isQuitRequested = false;
};


class IPlayer
{
public:
    virtual std::string getName() const = 0;

    virtual bool isLocalPlayer() const = 0;

    virtual Player getPlayerType() const = 0;

    virtual InputRequest getInput() = 0;
};

