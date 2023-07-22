#pragma once
#include "IPlayer.h"
#include "Core/CoreTypes.h"
#include "Core/EventBus.h"
#include "Game/Events/Events.h"
#include "Game/GameConfig.h"
#include "Game/BattleSeaGame.h"

#include <deque>
#include <algorithm>
#include <random>
#include <thread>
#include <chrono>
#include <assert.h>


class SillyBotPlayer : public IPlayer
{
public:
    SillyBotPlayer(const Player player, const GameConfig& config, std::shared_ptr<EventBus>& bus)
        : m_currentPlayer(player)
        , m_eventBus(bus)
        , m_opponentGrid{} // default with all cells concealed
    {
        // TODO [optional] need to guarantee thread-safety ideally, but while view doesn't push any event we are totally safe
        //m_eventBus->subscribe<events::LocalShotErrorEvent>([](const std::any _)
        //    {
        //        // TODO process incorrect shoot if need
        //    });

        m_shotMissedEventHandleId = m_eventBus->subscribe<events::ShotMissedEvent>([this](const std::any& anyEvent)
            {
                const auto event = any_cast<events::ShotMissedEvent>(anyEvent);
                if (event.shootingPlayer != getPlayerType())
                {
                    return;
                }

                m_opponentGrid.at(event.shot) = CellState::Missed;

                // switch to random shooting
            });
        m_shipDamagedEventHandleId = m_eventBus->subscribe<events::ShipDamagedEvent>([this](const std::any& anyEvent)
            {
                const auto event = any_cast<events::ShipDamagedEvent>(anyEvent);
                if (event.injuredPlayer != getOppositePlayer(getPlayerType()))
                {
                    return;
                }
                m_opponentGrid.at(event.shot) = CellState::Damaged;

                // switch to state after hit state
            });
        m_shipDestroyedEventHandleId = m_eventBus->subscribe<events::ShipDestroyedEvent>([this](const std::any& anyEvent)
            {
                const auto event = any_cast<events::ShipDestroyedEvent>(anyEvent);
                if (event.injuredPlayer != getOppositePlayer(getPlayerType()))
                {
                    return;
                }
                
                // Mark the whole ship in destroyed
                for (const CellIndex& shipCell : event.ship.getOccupiedCells())
                {
                    m_opponentGrid.at(shipCell) = CellState::Destroyed;
                }

                for (const CellIndex& cellAround : event.surroundedCells)
                {
                    m_opponentGrid.at(cellAround) = CellState::Missed;
                }
                
                // analyze left ships
            });


        for (int i = 0; i < config.rowsCount; i++)
        {
            for (int j = 0; j < config.columnsCount; j++)
            {
                m_sequenceTurns.emplace_back(i, j);
            }
        }
        assert(!m_sequenceTurns.empty());

        std::random_device rd;
        std::mt19937 g(rd());

        std::shuffle(m_sequenceTurns.begin(), m_sequenceTurns.end(), g);
    }
    ~SillyBotPlayer()
    {
        m_eventBus->unsubscribe(m_shotMissedEventHandleId);
        m_eventBus->unsubscribe(m_shipDamagedEventHandleId);
        m_eventBus->unsubscribe(m_shipDestroyedEventHandleId);
    }

    virtual std::string getName() const override
    {
        return std::string("Bot");
    }
    virtual bool isLocalPlayer() const override
    {
        return false;
    }
    virtual Player getPlayerType() const override
    {
        return m_currentPlayer;
    }
    virtual InputRequest getInput() override
    {
        InputRequest turn;

        const Player opponent = getOppositePlayer(getPlayerType());
        CellState state = CellState::Concealed;
        do
        {
            if (m_sequenceTurns.empty())
            {
                return InputRequest{};
            }

            turn.shotCell = m_sequenceTurns[0];
            m_sequenceTurns.pop_front();

            state = m_opponentGrid.at(*turn.shotCell);
        } while (state != CellState::Concealed && state != CellState::Ship);

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(250ms);
        return turn;
    }

protected:
    Player m_currentPlayer;
    std::shared_ptr<EventBus> m_eventBus;

    GameGrid m_opponentGrid;

private:
    std::deque<CellIndex> m_sequenceTurns;

    ListenerHandleId m_shotMissedEventHandleId;
    ListenerHandleId m_shipDamagedEventHandleId;
    ListenerHandleId m_shipDestroyedEventHandleId;
};

