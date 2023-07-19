#pragma once
#include "Controllers.h"
#include "Game/GameConfig.h"
#include "Game/BattleSeaGame.h"
#include "Views.h"

#include <memory>


class BattleSeaGame;

class IBattleSeaFactory
{
public:
    virtual std::unique_ptr<BattleSeaGame> createGame(std::shared_ptr<EventBus>& bus) = 0;
    virtual std::shared_ptr<IController> createController(
        std::unique_ptr<BattleSeaGame>&& game,
        std::shared_ptr<EventBus>& bus
    ) = 0;
};


class ClassicTerminalBattleSeaFactory : public IBattleSeaFactory
{
public:
    virtual std::unique_ptr<BattleSeaGame> createGame(std::shared_ptr<EventBus>& bus) override
    {
        const GameConfig& gameConfig = DEFAULT_GAME_CONFIG;
        return std::make_unique<BattleSeaGame>(gameConfig, bus);
    }
    virtual std::shared_ptr<IController> createController(std::unique_ptr<BattleSeaGame>&& game, std::shared_ptr<EventBus>& bus) override
    {
        std::unique_ptr<IBattleSeaView> terminalRenderer(new TerminalView (bus));
        return std::make_shared<TerminalController>(std::move(game), std::move(terminalRenderer), bus);
    }
};

class HasbroTerminalBattleSeaFactory : public ClassicTerminalBattleSeaFactory
{
public:
    virtual std::unique_ptr<BattleSeaGame> createGame(std::shared_ptr<EventBus>& bus) override
    {
        const GameConfig& gameConfig = HASBRO_GAME_CONFIG;
        return std::make_unique<BattleSeaGame>(gameConfig, bus);
    }
};


class FactoryInterface
{
public:
    template<class T>
    static std::unique_ptr<IBattleSeaFactory> getFactory()
    {
        return std::make_unique<T>();
    }
};
