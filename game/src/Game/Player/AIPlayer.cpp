#include "AIPlayer.h"

#include <string>
#include <random>
#include "Core/CoreTypes.h"
#include "Game/GameConfig.h"
#include <iostream> //DEBUG


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
    std::cout << "random shot" << std::endl; //DEBUG
    if (!m_lastHits.empty())
    {
        m_lvl = Level::ShotAfterHit;
        return shotAfterHit();
    }
    const Player opponent = getOppositePlayer(getPlayerType());

    CellState state = CellState::Concealed;

    std::cout << "before permission grids" << std::endl; //DEBUG
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
    std::cout << "after permission grids: " << permissionCells.size() << std::endl; //DEBUG
    std::uniform_int_distribution<int> dist(0, permissionCells.size() - 1);

    CellIndex cell(permissionCells.at(dist(mt)));
    std::cout << "after permission grids: cell " << cell.toString() << std::endl; //DEBUG
    if (m_gameInstance->getPlayerGridCellState(opponent, cell) == CellState::Ship)
    {
        m_lastHits.push_back(cell);
    }
    std::cout << "after check cell state: " << permissionCells.size() << std::endl; //DEBUG
    InputRequest turn;
    turn.shotCell = cell;
    std::cout << "shoot cell: " << permissionCells.size() << std::endl; //DEBUG
    return turn;
}

InputRequest AIPlayer::shotAfterHit()
{
    std::cout << "shot after hit" << std::endl; //DEBUG
    const Player opponent = getOppositePlayer(getPlayerType());
    for (auto& h : m_lastHits)
    {
        if (m_gameInstance->getPlayerGridCellState(opponent, h) == CellState::Destroyed)
        {
            std::cout << "change state" << std::endl; //DEBUG
            m_lastHits.clear();
            m_lvl = Level::RandomShot;
            return randomShot();
        }
    }


    std::vector<CellIndex> possibleCellDirections;
    if (m_lastHits.size() > 1)
    {
        std::cout << "shot after hit: hits " << m_lastHits.size() << std::endl; //DEBUG
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
        std::cout << "before random" << std::endl; //DEBUG
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
        if (m_gameInstance->getPlayerGridCellState(opponent, permissionCells.at(0)) == CellState::Ship)
        {
            m_lastHits.push_back(permissionCells.at(0));
        }
        turn.shotCell = permissionCells.at(0);
    }


    return turn;
}

InputRequest AIPlayer::middleRandomShot()
{
    // TODO: write function with ships possible location
    return randomShot();
}