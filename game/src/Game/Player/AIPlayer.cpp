#include "AIPlayer.h"

#include <string>
#include <thread>
#include <chrono>

#include "Core/CoreTypes.h"
#include "Game/GameConfig.h"
#include "Game/BattleSeaGame.h"
#include <iostream> //DEBUG
AIPlayer::AIPlayer(const Player player, const BattleSeaGame* game)
    : m_currentPlayer(player)
    , m_gameInstance(game)
    , m_lastHits{}
    , m_state(AIPlayerState::RandomShooting)
    , m_enemiesShips(game->getAppliedConfig().numberOfMultiDeckShips)
    , rd {}
    , mt(rd())
{
    std::sort(m_enemiesShips.begin(), m_enemiesShips.end(), std::greater<uint8_t>());
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
    InputRequest turn;
    turn.shotCell = getShootingCell();

    simulateThinking();

    return turn;
}

CellIndex AIPlayer::getShootingCell()
{
    switch (m_state)
    {
    case AIPlayerState::RandomShooting:
    {
        return getRandomShootingCell();
    }
    case AIPlayerState::ShootingAfterHit:
    {
        return getShootingAfterHitCell();
    }
    case AIPlayerState::MiddleRandomShooting:
    {
        return getMiddleRandomShootingCell();
    }
    default:
    {
        assert(false && "Unprocessed AIPlayerState. Take care of it!");
    }
    }
}

CellIndex AIPlayer::getRandomShootingCell()
{
    if (!m_lastHits.empty())
    {
        m_state = AIPlayerState::ShootingAfterHit;
        return getShootingAfterHitCell();
    }
    const Player opponent = getOppositePlayer(getPlayerType());

    // TODO should be removed after event-based approach implemented
    assert(m_gameInstance);
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

    if (permissionCells.size() < (m_gameInstance->getAppliedConfig().columnsCount * m_gameInstance->getAppliedConfig().rowsCount) / 2)
    {
        m_state = AIPlayerState::MiddleRandomShooting;
        return getMiddleRandomShootingCell();
    }

    std::uniform_int_distribution<int> dist(0, (int)permissionCells.size() - 1);

    CellIndex cell(permissionCells.at(dist(mt)));

    // TODO: HACK will be implemented after events about hit and missed shot are ready
    if (m_gameInstance->getPlayerGridCellState(opponent, cell) == CellState::Ship)
    {
        m_lastHits.push_back(cell);
    }
    return cell;
}

CellIndex AIPlayer::getShootingAfterHitCell()
{
    const Player opponent = getOppositePlayer(getPlayerType());
    for (auto& hit : m_lastHits)
    {
        if (m_gameInstance->getPlayerGridCellState(opponent, hit) == CellState::Destroyed)
        {
            auto rdestroyedShip = std::remove(m_enemiesShips.begin(), m_enemiesShips.end(), (uint8_t)m_lastHits.size());
            m_enemiesShips.erase(rdestroyedShip, m_enemiesShips.end());
            m_lastHits.clear();
            m_state = AIPlayerState::RandomShooting;

            return getRandomShootingCell();
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
    else // m_lastHits.size() <= 1
    {
        assert(m_lastHits.size() == 1);
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
            m_gameInstance->getPlayerGridCellState(opponent, c) == CellState::Concealed ||
            m_gameInstance->getPlayerGridCellState(opponent, c) == CellState::Damaged)
        {
            continue;
        }

        allowedCells.push_back(c);
    }

    InputRequest turn;
    CellIndex cell{ allowedCells.at(0) };

    std::uniform_int_distribution<int> dist(0, (int)allowedCells.size() - 1);
    cell = allowedCells.at(dist(mt));

    if (m_gameInstance->getPlayerGridCellState(opponent, cell) == CellState::Ship)
    {
        m_lastHits.push_back(cell);
    }

    return cell;
}

CellIndex AIPlayer::getMiddleRandomShootingCell()
{
    if (!m_lastHits.empty())
    {
        m_state = AIPlayerState::ShootingAfterHit;
        return getShootingAfterHitCell();
    }
    const Player opponent = getOppositePlayer(getPlayerType());

    // TODO should be removed after event-based approach implemented
    assert(m_gameInstance);
    auto gameGrid = m_gameInstance->getPlayerGridInfo(opponent);
    std::vector<std::vector<CellIndex>> permissionCellsGroups;
    for (int i = 0; i < m_gameInstance->getAppliedConfig().rowsCount; ++i)
    {
        std::vector<CellIndex> permissionCells;
        for (int j = 0; j < m_gameInstance->getAppliedConfig().columnsCount; ++j)
        {
            if (gameGrid.data[i][j] != CellState::Damaged &&
                gameGrid.data[i][j] != CellState::Missed &&
                gameGrid.data[i][j] != CellState::Destroyed)
            {
                permissionCells.push_back(CellIndex(i, j));
            }
            else
            {
                if (!permissionCells.empty())
                {
                    if (static_cast<uint8_t>(permissionCells.size()) >= *(m_enemiesShips.begin()))
                    {
                        permissionCellsGroups.push_back(permissionCells);
                    }
                    permissionCells.clear();
                }
            }
        }
    }

    for (int i = 0; i < m_gameInstance->getAppliedConfig().columnsCount; ++i)
    {
        std::vector<CellIndex> permissionCells;
        for (int j = 0; j < m_gameInstance->getAppliedConfig().rowsCount; ++j)
        {
            if (gameGrid.data[j][i] != CellState::Damaged &&
                gameGrid.data[j][i] != CellState::Missed &&
                gameGrid.data[j][i] != CellState::Destroyed)
            {
                permissionCells.push_back(CellIndex(j, i));
            }
            else
            {
                if (!permissionCells.empty())
                {
                    if (permissionCells.size() >= *std::max_element(m_enemiesShips.begin(), m_enemiesShips.end()))
                    {
                        permissionCellsGroups.push_back(permissionCells);
                    }
                    permissionCells.clear();
                }
            }
        }
    }


    std::uniform_int_distribution<int> dist(0, (int)permissionCellsGroups.size() - 1);
    
    std::vector<CellIndex> cells(permissionCellsGroups.at(dist(mt)));
    std::uniform_int_distribution<int> dist2(0, (int)cells.size() - 1);

    CellIndex cell(cells.at(dist2(mt)));


    // TODO: HACK will be implemented after events about hit and missed shot are ready
    if (m_gameInstance->getPlayerGridCellState(opponent, cell) == CellState::Ship)
    {
        m_lastHits.push_back(cell);
    }
    return cell;
}


void AIPlayer::simulateThinking()
{
    using namespace std::chrono_literals;
    constexpr auto thinkingTime = 350ms;
    std::this_thread::sleep_for(thinkingTime);
}
