#pragma once
#include "IPlayer.h"
#include "Core/CoreTypes.h"
#include "Game/GameConfig.h"
#include "Core/CharUtilities.h"

#include <deque>

class SillyBotPlayer : public IPlayer
{
public:
    SillyBotPlayer(const Player player, const std::shared_ptr<IBattleSeaGame>& game)
        : m_currentPlayer(player)
        , m_gameInstance(game)
    {
        // TODO get value from applied config
        for (const char c : ROW_AXIS_NAMES)
        {
            for (int i = 0; i < CLASSIC_GRID_COLUMN_COUNT; i++)
            {
                const CellIndex cell = CellIndex(char_utilities::letterCharToInt(c), i);
                m_sequenceTurns.push_back(cell);
            }
        }
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
    virtual std::optional<InputRequest> getInput() override
    {
        if (m_sequenceTurns.empty())
        {
            return std::nullopt;
        }

        const Player opponent = getOppositePlayer(getPlayerType());

        // REDO in do-while
        CellIndex cell = m_sequenceTurns[0];
        m_sequenceTurns.pop_front();
        // IS IT CHEATING? do we need to introduce separate local grid and fill it by info from shooting?
        CellState state = m_gameInstance->getPlayerGridCellState(opponent, cell);
        while (state != CellState::Concealed && state != CellState::Ship)
        {
            if (m_sequenceTurns.empty())
            {
                return std::nullopt;
            }

            cell = m_sequenceTurns[0];
            m_sequenceTurns.pop_front();
            state = m_gameInstance->getPlayerGridCellState(opponent, cell);
        }

        InputRequest turn;
        turn.shotCell = cell;
        return turn;
    }

protected:
    Player m_currentPlayer;
    const std::shared_ptr<IBattleSeaGame> m_gameInstance;

private:
    std::deque<CellIndex> m_sequenceTurns;
};

