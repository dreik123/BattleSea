#pragma once
#include "Controllers.h"
#include "Game/GameConfig.h"
#include "Game/WarShipGenerators.h"
#include "Game/BattleSeaGame.h"
#include "Views.h"

#include <memory>

class IBattleSeaGame;

class IBattleSeaFactory
{
public:
    virtual std::shared_ptr<IBattleSeaGame> createGame() = 0;
    virtual std::shared_ptr<IBattleSeaView> createPresenter(std::shared_ptr<IBattleSeaGame>& game) = 0;
    virtual std::shared_ptr<IController> createController(std::shared_ptr<IBattleSeaGame>& game, std::shared_ptr<IBattleSeaView>& view) = 0;
};


class ClassicConsoleBattleSeaFactory : public IBattleSeaFactory
{
public:
    virtual std::shared_ptr<IBattleSeaGame> createGame() override
    {
        const GameConfig& gameConfig = DEFAULT_GAME_CONFIG;
        auto generator = std::make_unique<WarShipGenerator>();
        return std::make_shared<BattleSeaGame>(std::move(generator), gameConfig);
    }
    virtual std::shared_ptr<IBattleSeaView> createPresenter(std::shared_ptr<IBattleSeaGame>& game) override
    {
        return std::make_shared<TerminalView>(game);
    }
    virtual std::shared_ptr<IController> createController(std::shared_ptr<IBattleSeaGame>& game, std::shared_ptr<IBattleSeaView>& view) override
    {
        return std::make_shared<GameController>(game, view);
    }
};

class HasbroConsoleBattleSeaFactory : public ClassicConsoleBattleSeaFactory
{
public:
    virtual std::shared_ptr<IBattleSeaGame> createGame() override
    {
        const GameConfig& gameConfig = HASBRO_GAME_CONFIG;
        auto generator = std::make_unique<WarShipGenerator>();
        return std::make_shared<BattleSeaGame>(std::move(generator), gameConfig);
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
