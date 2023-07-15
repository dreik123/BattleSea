#pragma once
#include "IPlayer.h"
#include "Game/GameInterfaces.h"
#include <vector>
#include <memory>
#include <random>


enum class AIPlayerState
{
    RandomShooting,
    ShootingAfterHit,
    MiddleRandomShooting,
};


class AIPlayer : public IPlayer
{
public:
    AIPlayer(const Player player, const std::shared_ptr<IBattleSeaGame>& game);

    virtual std::string getName() const override;

    virtual bool isLocalPlayer() const override;

    [[nodiscard]] virtual Player getPlayerType() const override;

    [[nodiscard]] virtual InputRequest getInput() override;

private:
    InputRequest playerState();

    InputRequest randomShooting();

    InputRequest shootingAfterHit();

    InputRequest middleRandomShooting();

protected:
    Player m_currentPlayer;
    const std::shared_ptr<IBattleSeaGame> m_gameInstance;

private:
    std::vector<CellIndex> m_lastHits;
    AIPlayerState m_lvl;
    std::random_device rd;
    std::mt19937 mt;
};

