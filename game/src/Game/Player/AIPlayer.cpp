#include "AIPlayer.h"

#include <string>
#include <random>
#include "Core/CoreTypes.h"
#include "Game/GameConfig.h"


AIPlayer::AIPlayer(const Player player, const std::shared_ptr<IBattleSeaGame>& game)
    : m_currentPlayer(player)
    , m_gameInstance(game)
{
    m_lvl = Level::RandomShot;
}

bool AIPlayer::isLocalPlayer() const
{
    return false;
}

Player AIPlayer::getPlayerType() const
{
    return m_currentPlayer;
}

std::string AIPlayer::getName() const
{
    return std::string("Bot");
}

InputRequest AIPlayer::getInput()
{
    return processEvent();
}

InputRequest AIPlayer::processEvent()
{
    InputRequest turn;
    switch (m_lvl)
    {
    case Level::RandomShot:
    {
        return randomShot();
    }
    case Level::ShotAfterHit:
    {
        return shotAfterHit();
    }
    case Level::MiddleRandomShot:
    {
        return middleRandomShot();
    }
    default:
    {
        return turn;
    }
    }
}

InputRequest AIPlayer::randomShot()
{
    const Player opponent = getOppositePlayer(getPlayerType());

    CellState state = CellState::Concealed;

    auto gamegrid = m_gameInstance->getPlayerGridInfo(opponent);
    std::vector<CellIndex> permissionCells;
    for (int i = 0; i < gamegrid.data.size(); ++i)
    {
        for (int j = 0; j < gamegrid.data[i].size(); ++j)
        {
            if (gamegrid.data[i][j] != CellState::Damaged &&
                gamegrid.data[i][j] != CellState::Missed &&
                gamegrid.data[i][j] != CellState::Destroyed)
            {
                permissionCells.push_back(CellIndex(i, j));
            }
        }
    }

    std::random_device rd;
    std::mt19937 mt(rd());

    std::uniform_int_distribution<int> dist(0, permissionCells.size() - 1);

    CellIndex cell(permissionCells.at(dist(mt)));

    if (m_gameInstance->getPlayerGridCellState(opponent, cell) == CellState::Ship)
    {
        m_lastHits.push_back(cell);
    }

    InputRequest turn;
    turn.shotCell = cell;
    return turn;
}

InputRequest AIPlayer::shotAfterHit()
{
    InputRequest turn;


    return turn;
}

InputRequest AIPlayer::middleRandomShot()
{
    // TODO: write function with ships possible location
    return randomShot();
}