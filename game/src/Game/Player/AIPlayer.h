#pragma once
#include "IPlayer.h"
#include <vector>
#include <memory>
#include "Game/GameInterfaces.h"

enum class Level
{
    RandomShot,
    ShotAfterHit,
    MiddleRandomShot,
};


class AIPlayer : public IPlayer
{
public:
    AIPlayer(const Player player, const std::shared_ptr<IBattleSeaGame>& game);

    virtual std::string getName() const override;

    virtual bool isLocalPlayer() const override;

    virtual Player getPlayerType() const override;
    virtual InputRequest getInput() override;

    InputRequest processEvent();

    InputRequest randomShot();

    InputRequest shotAfterHit();

    InputRequest middleRandomShot();

protected:
    Player m_currentPlayer;
    const std::shared_ptr<IBattleSeaGame> m_gameInstance;

private:
    std::vector<CellIndex> m_lastHits;
    Level m_lvl;
};

