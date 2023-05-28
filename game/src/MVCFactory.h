#pragma once
#include "Controllers.h"
#include "GameConfig.h"
#include "Views.h"

#include <memory>

struct IBattleSeaGame;

// TODO declare a factory class with these methods

std::shared_ptr<IBattleSeaGame> CreateGame(const GameConfig&) { return nullptr; }

std::shared_ptr<IBattleSeaView> CreatePresenter(std::shared_ptr<IBattleSeaGame>& InGame)
{
    return std::make_shared<TerminalView>(InGame);
}

std::shared_ptr<IController> CreateController(std::shared_ptr<IBattleSeaGame>& InGame, std::shared_ptr<IBattleSeaView>& InView)
{
    return std::make_shared<GameController>(InGame, InView);
}
