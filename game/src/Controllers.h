#pragma once
#include <memory>
#include <array>

#include "Core/CoreTypes.h"
#include "Game/Player/IPlayer.h"
#include "Game/WarShipGenerators.h"

class BattleSeaGame;
class IBattleSeaView;

class EventBus;


// DS: It's expected to communicate between MVC entries via 'event-based' subsystem, 
// but for this moment old-school approach should cover our needs
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

    virtual void loopGame() override;

private:
    IPlayer& getCurrentPlayer(const Player player) const;

    const std::vector<WarShip> getShipsFromPlayer(const Player player);

    // TODO consider these methods as listeners for corresponding events
    bool onStartScreen();
    bool onShipsSetup();
    bool onBattleStarted();
    bool onBattleFinished();

protected:
    std::unique_ptr<BattleSeaGame> m_game;
    std::unique_ptr<IBattleSeaView> m_renderer;

private:
    std::array<std::unique_ptr<IPlayer>, 2> m_players;
    std::unique_ptr<IWarShipGenerator> m_shipsGenerator;
    std::shared_ptr<EventBus> m_eventBus;
};
