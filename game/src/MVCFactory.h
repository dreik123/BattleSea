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
    virtual std::shared_ptr<BattleSeaGame> createGame(std::shared_ptr<EventBus>& bus) = 0;
    virtual std::shared_ptr<IBattleSeaView> createPresenter(std::weak_ptr<BattleSeaGame> game, std::shared_ptr<EventBus>& bus) = 0;
    virtual std::shared_ptr<IController> createController(
        std::weak_ptr<BattleSeaGame> game,
        std::shared_ptr<IBattleSeaView>& view, // need to think
        std::shared_ptr<EventBus>& bus
    ) = 0;
};


class ClassicConsoleBattleSeaFactory : public IBattleSeaFactory
{
public:
    virtual std::shared_ptr<BattleSeaGame> createGame(std::shared_ptr<EventBus>& bus) override
    {
        const GameConfig& gameConfig = DEFAULT_GAME_CONFIG;
        return std::make_shared<BattleSeaGame>(gameConfig, bus);
    }
    virtual std::shared_ptr<IBattleSeaView> createPresenter(std::weak_ptr<BattleSeaGame> game, std::shared_ptr<EventBus>& bus) override
    {
        return std::make_shared<TerminalView>(game, bus);
    }
    virtual std::shared_ptr<IController> createController(std::weak_ptr<BattleSeaGame> game, std::shared_ptr<IBattleSeaView>& view, std::shared_ptr<EventBus>& bus) override
    {
        return std::make_shared<GameController>(game, view, bus);
    }
};

class HasbroConsoleBattleSeaFactory : public ClassicConsoleBattleSeaFactory
{
public:
    virtual std::shared_ptr<BattleSeaGame> createGame(std::shared_ptr<EventBus>& bus) override
    {
        const GameConfig& gameConfig = HASBRO_GAME_CONFIG;
        return std::make_shared<BattleSeaGame>(gameConfig, bus);
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
