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
    virtual std::shared_ptr<IBattleSeaGame> CreateGame() = 0;
    virtual std::shared_ptr<IBattleSeaView> CreatePresenter(std::shared_ptr<IBattleSeaGame>& InGame) = 0;
    virtual std::shared_ptr<IController> CreateController(std::shared_ptr<IBattleSeaGame>& InGame, std::shared_ptr<IBattleSeaView>& InView) = 0;
};


struct ClassicConsoleBattleSeaFactory : public IBattleSeaFactory
{
    virtual std::shared_ptr<IBattleSeaGame> CreateGame() override
    {
        const GameConfig& gameConfig = DefaultGameConfig;
        auto generator = std::make_unique<PredefinedClassicWarShipGenerator>();
        return std::make_shared<BattleSeaGame>(std::move(generator), gameConfig);
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

struct HasbroConsoleBattleSeaFactory : public ClassicConsoleBattleSeaFactory
{
    virtual std::shared_ptr<IBattleSeaGame> CreateGame() override
    {
        const GameConfig& gameConfig = HasbroGameConfig;
        auto generator = std::make_unique<PredefinedClassicWarShipGenerator>();
        return std::make_shared<BattleSeaGame>(std::move(generator), gameConfig);
    }
};


class FactoryInterface
{
public:
    // TODO extend functionality with polimophic opportunity if need
    static std::unique_ptr<IBattleSeaFactory> GetFactory()
    {
        return std::make_unique<ClassicConsoleBattleSeaFactory>();
    }
};
