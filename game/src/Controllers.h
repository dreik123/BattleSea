#pragma once
#include <memory>
#include <array>

#include "Core/CoreTypes.h"
#include "Game/Player/IPlayer.h"
#include "Game/WarShipGenerators.h"

class IBattleSeaGame;
class IBattleSeaView;


// DS: It's expected to communicate between MVC entries via 'event-based' subsystem, 
// but for this moment old-school approach should cover our needs
class IController
{
public:
    virtual void runGame() = 0;
};

class GameController : public IController 
{
public:
    GameController(std::shared_ptr<IBattleSeaGame>& game, std::shared_ptr<IBattleSeaView>& view);

    virtual void runGame() override;

private:
    IPlayer& getCurrentPlayer(const Player player) const;

protected:
    std::shared_ptr<IBattleSeaGame> m_game;
    std::shared_ptr<IBattleSeaView> m_view;

private:
    std::array<std::unique_ptr<IPlayer>, 2> m_players;
    std::unique_ptr<IWarShipGenerator> m_shipsGenerator;
};
