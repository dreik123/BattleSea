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
    virtual std::shared_ptr<IBattleSeaGame> createGame() = 0;
    virtual std::shared_ptr<IBattleSeaView> createPresenter(std::shared_ptr<IBattleSeaGame>& game) = 0;
    virtual std::shared_ptr<IController> createController(std::shared_ptr<IBattleSeaGame>& game, std::shared_ptr<IBattleSeaView>& view) = 0;
};


struct ClassicConsoleBattleSeaFactory : public IBattleSeaFactory
{
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

struct HasbroConsoleBattleSeaFactory : public ClassicConsoleBattleSeaFactory
{
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
    // TODO extend functionality with polimophic opportunity if need
    static std::unique_ptr<IBattleSeaFactory> getFactory()
    {
        return std::make_unique<ClassicConsoleBattleSeaFactory>();
    }
};
