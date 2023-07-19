#pragma once
#include <memory>
#include <array>
#include <thread>

#include "Core/CoreTypes.h"
#include "Game/Player/IPlayer.h"
#include "Game/WarShipGenerators.h"

class BattleSeaGame;
class IBattleSeaView;

class EventBus;


class IController
{
public:
    virtual void loopGame() = 0;
};

class TerminalController : public IController
{
public:
    TerminalController(
        std::unique_ptr<BattleSeaGame>&& game,
        std::unique_ptr<IBattleSeaView>&& view,
        std::shared_ptr<EventBus>& bus);

    ~TerminalController();

    virtual void loopGame() override;

private:
    IPlayer& getCurrentPlayer(const Player player) const;

    const std::vector<WarShip> getShipsFromPlayer(const Player player);

    // TODO [optional] consider these methods as listeners for corresponding events
    bool onStartScreen();
    bool onShipsSetup();
    bool onBattleStarted();
    bool onBattleFinished();

protected:
    std::unique_ptr<BattleSeaGame> m_game;
    std::shared_ptr<EventBus> m_eventBus;

private:
    std::array<std::unique_ptr<IPlayer>, 2> m_players;
    std::unique_ptr<IWarShipGenerator> m_shipsGenerator; // TODO need to think about delegating this to game (maybe static)

    std::jthread m_renderThread;
};
