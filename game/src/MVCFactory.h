#pragma once
#include "Controllers.h"
#include "Game/GameConfig.h"
#include "Game/WarShipGenerators.h"
#include "Game/BattleSeaGame.h"
#include "Views.h"

#include <memory>

struct IBattleSeaGame;

struct IBattleSeaFactory
{
    virtual std::shared_ptr<IBattleSeaGame> CreateGame(const GameConfig&) = 0;
    virtual std::shared_ptr<IBattleSeaView> CreatePresenter(std::shared_ptr<IBattleSeaGame>& InGame) = 0;
    virtual std::shared_ptr<IController> CreateController(std::shared_ptr<IBattleSeaGame>& InGame, std::shared_ptr<IBattleSeaView>& InView) = 0;
};


struct ConsoleBattleSeaFactory : public IBattleSeaFactory
{
    virtual std::shared_ptr<IBattleSeaGame> CreateGame(const GameConfig&) override
    {
        auto generator = new PredefinedClassicWarShipGenerator();
        return std::make_shared<BattleSeaGame>(generator);
    }
    virtual std::shared_ptr<IBattleSeaView> CreatePresenter(std::shared_ptr<IBattleSeaGame>& InGame) override
    {
        return std::make_shared<TerminalView>(InGame);
    }
    virtual std::shared_ptr<IController> CreateController(std::shared_ptr<IBattleSeaGame>& InGame, std::shared_ptr<IBattleSeaView>& InView) override
    {
        return std::make_shared<GameController>(InGame, InView);
    }
};


class FactoryInterface
{
public:
    // TODO extend functionality with polimophic opportunity if need
    static std::unique_ptr<IBattleSeaFactory> GetFactory()
    {
        return std::make_unique<ConsoleBattleSeaFactory>();
    }
};
