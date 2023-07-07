#pragma once
#include "IPlayer.h"
#include "Core/CoreTypes.h"
#include "Game/GameConfig.h"
#include "Game/BattleSeaGame.h"

#include <deque>
#include <algorithm>
#include <random>
#include <thread>
#include <chrono>

class SillyBotPlayer : public IPlayer
{
public:
    SillyBotPlayer(const Player player, const std::shared_ptr<BattleSeaGame>& game)
        : m_currentPlayer(player)
        , m_gameInstance(game)
    {
        const GameConfig& config = m_gameInstance->getAppliedConfig();
        for (int i = 0; i < config.rowsCount; i++)
        {
            for (int j = 0; j < config.columnsCount; j++)
            {
                m_sequenceTurns.emplace_back(i, j);
            }
        }

        std::random_device rd;
        std::mt19937 g(rd());

        std::shuffle(m_sequenceTurns.begin(), m_sequenceTurns.end(), g);
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
        const Player opponent = getOppositePlayer(getPlayerType());

        CellState state = CellState::Concealed;
        InputRequest turn;
        do
        {
            if (m_sequenceTurns.empty())
            {
                return InputRequest{};
            }

            turn.shotCell = m_sequenceTurns[0];
            m_sequenceTurns.pop_front();

            // That's CHEATING! Ideally to use separate local grid and fill it by info from shooting
            state = m_gameInstance->getPlayerGridCellState(opponent, *turn.shotCell);
        } while (state != CellState::Concealed && state != CellState::Ship);

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(500ms);
        return turn;
    }

protected:
    Player m_currentPlayer;
    const std::shared_ptr<BattleSeaGame> m_gameInstance;

private:
    std::deque<CellIndex> m_sequenceTurns;
};

