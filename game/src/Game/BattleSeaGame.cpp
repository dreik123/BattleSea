#include "BattleSeaGame.h"
#include "Game/WarShipGenerators.h"
#include "Game/WarShip.h"


BattleSeaGame::BattleSeaGame(const GameConfig& config)
    : m_config(config)
    , m_playerGrids{}
    , m_playerShips{}
    , m_currentPlayer(Player::Invalid)
    , m_initialPlayer(Player::Invalid)
    , m_localPlayer(Player::Invalid)
    , m_hasGameFinished(false)
{
}

bool BattleSeaGame::initShipPositionsForPlayer(const Player player, const std::vector<WarShip>& ships)
{
    // TODO [MUST HAVE] validate ships position at least under some macro definition (debug for ex.)
    const bool isValidated = true;
    if (!isValidated)
    {
        return false;
    }

    m_playerShips[getIndexFromPlayer(player)] = ships;

    return true;
}

ShotError BattleSeaGame::shootThePlayerGridAt(const CellIndex& cell)
{
    assert(getCurrentPlayer() != Player::Invalid);

    const int oppositePlayerIndex = getIndexFromPlayer(getOppositePlayer(getCurrentPlayer()));
    GameGrid& oppositeGrid = m_playerGrids[oppositePlayerIndex];
    std::vector<WarShip>& ships = m_playerShips[oppositePlayerIndex];

    if (cell.x() < 0 || cell.x() >= m_config.rowsCount || cell.y() < 0 || cell.y() >= m_config.columnsCount)
    {
        // Invalid input from player (out of current grid size)
        return ShotError::OutOfGrid;
    }

    if (oppositeGrid.at(cell) != CellState::Concealed)
    {
        return ShotError::RepeatedShot;
    }

    bool playerSwitch = true;

    for (WarShip& ship : ships)
    {
        if (ship.isDestroyed() || !ship.doesOccupyTheCell(cell))
        {
            continue;
        }

        ship.shootShipAtCell(cell);

        if (!ship.isDestroyed())
        {
            setGridCellState(oppositeGrid, cell, CellState::Damaged);
        }
        else
        {
            // Mark the whole ship in destroyed
            for (const CellIndex& shipCell : ship.getOccupiedCells())
            {
                setGridCellState(oppositeGrid, cell, CellState::Destroyed);
            }

            // Wrap all surrounded cells of the ship in missed
            surroundDestroyedShip(oppositeGrid, ship);

            m_hasGameFinished = std::all_of(ships.cbegin(), ships.cend(), [](const WarShip& s)
                {
                    return s.isDestroyed();
                });
        }
        playerSwitch = false; // continue shooting
        break;
    }

    if (playerSwitch)
    {
        setGridCellState(oppositeGrid, cell, CellState::Missed);
        m_currentPlayer = getOppositePlayer(m_currentPlayer);
    }

    return ShotError::Ok;
}

bool BattleSeaGame::startGame(const GameStartSettings& settings)
{
    m_initialPlayer = settings.initialPlayer;
    m_currentPlayer = settings.initialPlayer;
    m_localPlayer = settings.localPlayer;

    if (!initShipPositionsForPlayer(Player::Player1, settings.shipsForPlayer1))
    {
        return false;
    }
    if (!initShipPositionsForPlayer(Player::Player2, settings.shipsForPlayer2))
    {
        return false;
    }

    return true;
}

bool BattleSeaGame::isGameOver() const
{
    return m_hasGameFinished;
}

Player BattleSeaGame::getCurrentPlayer() const
{
    return m_currentPlayer;
}

Player BattleSeaGame::getInitialPlayer() const
{
    return m_initialPlayer;
}

Player BattleSeaGame::getLocalPlayer() const
{
    return m_localPlayer;
}

const GameGrid BattleSeaGame::getPlayerGridInfo(const Player player) const
{
    // Create a copy of grid data to make the ship data visible for local player only
    const int playerIndex = getIndexFromPlayer(player);

    if (getLocalPlayer() != player)
    {
        return m_playerGrids[playerIndex];
    }

    GameGrid gridCopy = m_playerGrids[playerIndex];
    const std::vector<WarShip>& warShips = m_playerShips[playerIndex];

    for (const WarShip& ship : warShips)
    {
        for (const CellIndex& cell : ship.getOccupiedCells())
        {
            if (gridCopy.at(cell) == CellState::Concealed)
            {
                gridCopy.at(cell) = CellState::Ship;
            }
        }
    }

    return gridCopy;
}

CellState BattleSeaGame::getPlayerGridCellState(const Player player, const CellIndex& cell) const
{
    return getPlayerGridInfo(player).at(cell);
}

const GameConfig& BattleSeaGame::getAppliedConfig() const
{
    return m_config;
}

void BattleSeaGame::setGridCellState(GameGrid& outGrid, const CellIndex& cell, const CellState& state)
{
    outGrid.at(cell) = state;
}

void BattleSeaGame::surroundDestroyedShip(GameGrid& outGrid, const WarShip& ship)
{
    // Example of class template auto deduction (CTAD)
    constexpr std::array possibleDirections =
    {
        std::make_pair(-1, 0),      // up
        std::make_pair(-1, -1),     // up-left
        std::make_pair(-1, 1),      // up-right
        std::make_pair(0, -1),      // left
        std::make_pair(0, 1),       // right
        std::make_pair(1, 0),       // down
        std::make_pair(1, -1),      // down-left
        std::make_pair(1, 1),       // down-right
    };

    // Forehead solution
    for (const CellIndex& cell : ship.getOccupiedCells())
    {
        const auto& [x, y] = cell.asIndexesPair();
        for (const auto& [dX, dY] : possibleDirections)
        {
            const int newX = x + dX;
            const int newY = y + dY;
            if (newX < 0 || newY < 0 || newX >= CLASSIC_GRID_ROW_COUNT || newY >= CLASSIC_GRID_COLUMN_COUNT)
            {
                continue;
            }

            if (outGrid[newX][newY] == CellState::Concealed)
            {
                outGrid[newX][newY] = CellState::Missed;
            }
        }
    }
}
