#pragma once
#include "IPlayer.h"
#include <vector>
#include <memory>
#include <random>


enum class AIPlayerState
{
    RandomShooting,
    ShootingAfterHit,
    MiddleRandomShooting,
};

class BattleSeaGame;

class AIPlayer : public IPlayer
{
public:
    AIPlayer(const Player player, const BattleSeaGame* game);

    virtual std::string getName() const override;

    virtual bool isLocalPlayer() const override;

    [[nodiscard]] virtual Player getPlayerType() const override;

    [[nodiscard]] virtual InputRequest getInput() override;

private:
    CellIndex getShootingCell();

    CellIndex getRandomShootingCell();

    CellIndex getShootingAfterHitCell();

    CellIndex getMiddleRandomShootingCell();

    void simulateThinking();

protected:
    Player m_currentPlayer;
    const BattleSeaGame* m_gameInstance;

private:
    std::vector<CellIndex> m_lastHits;
    AIPlayerState m_state;
    std::random_device rd;
    std::mt19937 mt;
};

