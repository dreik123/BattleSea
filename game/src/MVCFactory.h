#pragma once
#include "Controllers.h"
#include "Game/GameConfig.h"
#include "Game/WarShipGenerators.h"
#include "Game/BattleSeaGame.h"
#include "Views.h"

#include <memory>

class BattleSeaGame;

class IBattleSeaFactory
{
public:
    virtual std::shared_ptr<BattleSeaGame> createGame() = 0;
    // TODO weak ptr
    virtual std::shared_ptr<IBattleSeaView> createPresenter(std::shared_ptr<BattleSeaGame>& game) = 0;
    virtual std::shared_ptr<IController> createController(std::shared_ptr<BattleSeaGame>& game, std::shared_ptr<IBattleSeaView>& view) = 0;
};


class ClassicConsoleBattleSeaFactory : public IBattleSeaFactory
{
public:
    virtual std::shared_ptr<BattleSeaGame> createGame() override
    {
        const GameConfig& gameConfig = DEFAULT_GAME_CONFIG;
        return std::make_shared<BattleSeaGame>(gameConfig);
    }
    virtual std::shared_ptr<IBattleSeaView> createPresenter(std::shared_ptr<BattleSeaGame>& game) override
    {
        return std::make_shared<TerminalView>(game);
    }
    virtual std::shared_ptr<IController> createController(std::shared_ptr<BattleSeaGame>& game, std::shared_ptr<IBattleSeaView>& view) override
    {
        return std::make_shared<GameController>(game, view);
    }
};

class HasbroConsoleBattleSeaFactory : public ClassicConsoleBattleSeaFactory
{
public:
    virtual std::shared_ptr<BattleSeaGame> createGame() override
    {
        const GameConfig& gameConfig = HASBRO_GAME_CONFIG;
        return std::make_shared<BattleSeaGame>(gameConfig);
    }
};


class FactoryInterface
{
public:
    // README Extend functionality with polimophic opportunity if need
    static std::unique_ptr<IBattleSeaFactory> getFactory()
    {
        return std::make_unique<ClassicConsoleBattleSeaFactory>();
    }
};
