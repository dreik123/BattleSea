#pragma once
#include "IPlayer.h"
#include "Core/EventBus.h"
#include "Core/CoreTypes.h"
#include "Game/GameConfig.h"
#include "Game/BattleSeaGame.h"
#include <vector>
#include <memory>
#include <random>


enum class AIPlayerState
{
    RandomShooting,
    ShootingAfterHit,
    MiddleGameRandomShooting,
};

class BattleSeaGame;

class AIPlayer : public IPlayer
{
public:
    AIPlayer(const Player player, const GameConfig& config, std::shared_ptr<EventBus>& bus);

    ~AIPlayer();

    virtual std::string getName() const override;

    virtual bool isLocalPlayer() const override;

    [[nodiscard]] virtual Player getPlayerType() const override;

    [[nodiscard]] virtual InputRequest getInput() override;

private:
    CellIndex getShootingCell();

    CellIndex getRandomShootingCell();

    CellIndex getShootingAfterHitCell();

    CellIndex getMiddleGameRandomShootingCell();

    void simulateThinking();

protected:
    Player m_currentPlayer;

private:
    GameGrid m_opponentGrid;
    std::shared_ptr<EventBus> m_eventBus;
    std::vector<CellIndex> m_lastHits;
    AIPlayerState m_state;
    std::vector<uint8_t> m_opponentShipDecks;
    std::random_device rd;
    std::mt19937 mt;
    const int m_middleGameCoeff;
    ListenerHandleId m_shotMissedEventHandleId;
    ListenerHandleId m_shipDamagedEventHandleId;
    ListenerHandleId m_shipDestroyedEventHandleId;
};

