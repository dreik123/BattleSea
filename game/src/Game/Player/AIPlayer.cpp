#include "AIPlayer.h"

#include <string>
#include <random>
#include "Core/CoreTypes.h"
#include "Game/GameConfig.h"


AIPlayer::AIPlayer(const Player player, const std::shared_ptr<IBattleSeaGame>& game)
    : m_currentPlayer(player)
    , m_gameInstance(game)
    , m_lastHits{}
    , m_lvl(Level::RandomShot)
    , rd {}
    , mt(rd())
{
    
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
    if (!m_lastHits.empty())
    {
        m_lvl = Level::ShotAfterHit;
        return shotAfterHit();
    }
    const Player opponent = getOppositePlayer(getPlayerType());

    CellState state = CellState::Concealed;

    auto gameGrid = m_gameInstance->getPlayerGridInfo(opponent);
    std::vector<CellIndex> permissionCells;
    for (int i = 0; i < gameGrid.data.size(); ++i)
    {
        for (int j = 0; j < gameGrid.data[i].size(); ++j)
        {
            if (gameGrid.data[i][j] != CellState::Damaged &&
                gameGrid.data[i][j] != CellState::Missed &&
                gameGrid.data[i][j] != CellState::Destroyed)
            {
                permissionCells.push_back(CellIndex(i, j));
            }
        }
    }

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
    const Player opponent = getOppositePlayer(getPlayerType());
    if (m_gameInstance->getPlayerGridCellState(opponent, m_lastHits.at(0)) == CellState::Destroyed)
    {
        m_lastHits.clear();
        m_lvl = Level::RandomShot;
        randomShot();
    }

    std::vector<CellIndex> possibleCellDirections;
    if (m_lastHits.size() > 1)
    {
        auto firstHit = m_lastHits.begin();
        auto lastHit = m_lastHits.end() - 1;

        if (firstHit->x() - lastHit->x() == 0)
        {
            if (firstHit->y() - lastHit->y() > 0)
            {
                possibleCellDirections.push_back(CellIndex(lastHit->x(), lastHit->y() - 1)); // up
                possibleCellDirections.push_back(CellIndex(lastHit->x(), firstHit->y() + 1)); // down
            }
            else
            {
                possibleCellDirections.push_back(CellIndex(lastHit->x(), firstHit->y() - 1)); // up
                possibleCellDirections.push_back(CellIndex(lastHit->x(), lastHit->y() + 1)); // down
            }
        }
        else
        {
            if (firstHit->x() - lastHit->x() > 0)
            {
                possibleCellDirections.push_back(CellIndex(lastHit->x() - 1, lastHit->y())); // left
                possibleCellDirections.push_back(CellIndex(firstHit->x() + 1, firstHit->y())); // right
            }
            else
            {
                possibleCellDirections.push_back(CellIndex(firstHit->x() - 1, firstHit->y())); // left
                possibleCellDirections.push_back(CellIndex(lastHit->x() + 1, lastHit->y())); // right
            }
        }
    }
    else
    {
        auto firstHit = m_lastHits.begin();
        possibleCellDirections.push_back(CellIndex(firstHit->x(), firstHit->y() - 1)); // up
        possibleCellDirections.push_back(CellIndex(firstHit->x(), firstHit->y() + 1)); // down
        possibleCellDirections.push_back(CellIndex(firstHit->x() - 1, firstHit->y())); // left
        possibleCellDirections.push_back(CellIndex(firstHit->x() + 1, firstHit->y())); // right
    }
    std::vector<CellIndex> permissionCells;
    for (auto& c : possibleCellDirections)
    {
        if (c.x() < 0 ||
            c.y() < 0 ||
            c.x() >= m_gameInstance->getAppliedConfig().columnsCount ||
            c.y() >= m_gameInstance->getAppliedConfig().rowsCount)
        {
            continue;
        }
        if (m_gameInstance->getPlayerGridCellState(opponent, c) == CellState::Missed ||
            m_gameInstance->getPlayerGridCellState(opponent, c) == CellState::Damaged)
        {
            continue;
        }

        permissionCells.push_back(c);
    }
    InputRequest turn;
    if (permissionCells.size() > 1)
    {
        std::uniform_int_distribution<int> dist(0, permissionCells.size() - 1);

        CellIndex cell(permissionCells.at(dist(mt)));

        if (m_gameInstance->getPlayerGridCellState(opponent, cell) == CellState::Ship)
        {
            m_lastHits.push_back(cell);
        }


        turn.shotCell = cell;
    }
    else
    {
        turn.shotCell = permissionCells.at(0);
    }


    return turn;
}

InputRequest AIPlayer::middleRandomShot()
{
    // TODO: write function with ships possible location
    return randomShot();
}