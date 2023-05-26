#pragma once
#include "Controllers.h"
#include "GameConfig.h"
#include "Views.h"

struct IBattleSeaGame;

// TODO declare a factory class with these methods
// TODO prefer unique_ptr instead of raw ptrs

IBattleSeaGame* CreateGame(const GameConfig&) { return nullptr; }

IBattleSeaView* CreatePresenter(const IBattleSeaGame* InGame)
{
    return new TerminalView(InGame);
}

IController* CreateController(IBattleSeaGame* InGame, IBattleSeaView* InView)
{
    return new GameController(InGame, InView);
}
