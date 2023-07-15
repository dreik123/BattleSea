#include "AIPlayer.h"

#include <string>

#include "Core/CoreTypes.h"
#include "Game/GameConfig.h"

AIPlayer::AIPlayer(const Player player, const std::shared_ptr<IBattleSeaGame>& game)
    : m_currentPlayer(player)
    , m_gameInstance(game)
    , m_lastHits{}
    , m_lvl(AIPlayerState::RandomShooting)
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
    return std::string("AI Bot");
}

InputRequest AIPlayer::getInput()
{
    return playerState();
}

InputRequest AIPlayer::playerState()
{
    InputRequest turn;
    switch (m_lvl)
    {
    case AIPlayerState::RandomShooting:
    {
        return randomShooting();
    }
    case AIPlayerState::ShootingAfterHit:
    {
        return shootingAfterHit();
    }
    case AIPlayerState::MiddleRandomShooting:
    {
        return middleRandomShooting();
    }
    default:
    {
        assert(m_lvl == AIPlayerState::RandomShooting);
        return turn;
    }
    }
}

InputRequest AIPlayer::randomShooting()
{
    if (!m_lastHits.empty())
    {
        m_lvl = AIPlayerState::ShootingAfterHit;
        return shootingAfterHit();
    }
    const Player opponent = getOppositePlayer(getPlayerType());

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

    // TODO: HACK will be implemented after events about hit and missed shot are ready
    if (m_gameInstance->getPlayerGridCellState(opponent, cell) == CellState::Ship)
    {
        m_lastHits.push_back(cell);
    }
    InputRequest turn;
    turn.shotCell = cell;
    return turn;
}

InputRequest AIPlayer::shootingAfterHit()
{
    const Player opponent = getOppositePlayer(getPlayerType());
    for (auto& hit : m_lastHits)
    {
        if (m_gameInstance->getPlayerGridCellState(opponent, hit) == CellState::Destroyed)
        {
            m_lastHits.clear();
            m_lvl = AIPlayerState::RandomShooting;
            return randomShooting();
        }
    }

    std::vector<CellIndex> possibleCellShots;
    if (m_lastHits.size() > 1)
    {
        auto firstHit = m_lastHits.begin();
        auto lastHit = m_lastHits.end() - 1;

        if (firstHit->x() - lastHit->x() == 0)
        {
            if (firstHit->y() - lastHit->y() > 0)
            {
                possibleCellShots = std::vector<CellIndex>{
                    CellIndex(lastHit->x(), lastHit->y() - 1), // up
                    CellIndex(lastHit->x(), firstHit->y() + 1) // down
                };
            }
            else
            {
                possibleCellShots = std::vector<CellIndex>{
                    CellIndex(lastHit->x(), firstHit->y() - 1), // up
                    CellIndex(lastHit->x(), lastHit->y() + 1) // down
                };
            }
        }
        else
        {
            if (firstHit->x() - lastHit->x() > 0)
            {
                possibleCellShots = std::vector<CellIndex>{
                    CellIndex(lastHit->x() - 1, lastHit->y()), // left
                    CellIndex(firstHit->x() + 1, firstHit->y()) // right
                };
            }
            else
            {
                possibleCellShots = std::vector<CellIndex>{
                    CellIndex(firstHit->x() - 1, firstHit->y()), // left
                    CellIndex(lastHit->x() + 1, lastHit->y()) // right
                };
            }
        }
    }
    else // m_lastHits.size() == 1
    {
        auto firstHit = m_lastHits.begin();
        possibleCellShots = std::vector<CellIndex>{
            CellIndex(firstHit->x(), firstHit->y() - 1), // up
            CellIndex(firstHit->x(), firstHit->y() + 1), // down
            CellIndex(firstHit->x() - 1, firstHit->y()), // left
            CellIndex(firstHit->x() + 1, firstHit->y()) // right
        };
        
    }
    std::vector<CellIndex> allowedCells;
    for (auto& c : possibleCellShots)
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

        allowedCells.push_back(c);
    }

    InputRequest turn;
    CellIndex cell{ allowedCells.at(0) };

    std::uniform_int_distribution<int> dist(0, allowedCells.size() - 1);
    cell = allowedCells.at(dist(mt));

    if (m_gameInstance->getPlayerGridCellState(opponent, cell) == CellState::Ship)
    {
        m_lastHits.push_back(cell);
    }

    turn.shotCell = cell;

    return turn;
}

InputRequest AIPlayer::middleRandomShooting()
{
    // TODO: write function with ships possible location
    return randomShooting();
}