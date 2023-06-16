#include "BattleSeaGame.h"
#include "Game/WarShipGenerators.h"
#include "Game/WarShip.h"


BattleSeaGame::BattleSeaGame(std::unique_ptr<IWarShipGenerator>&& generator, const GameConfig& config)
    : m_gridGenerator(std::move(generator))
    , m_config(config)
    , m_playerGrids{}
    , m_playerShips{}
    , m_currentPlayer(Player::Invalid)
    , m_initialPlayer(Player::Invalid)
    , m_localPlayer(Player::Invalid)
{
    for (GridData& gridData : m_playerGrids)
    {
        for (auto& gridRow : gridData)
        {
            for (CellState& cell : gridRow)
            {
                cell = CellState::Concealed;
            }
        }
    }
}

void BattleSeaGame::generateShipsForPlayer(const Player player)
{
    const std::vector<WarShip> warShips = m_gridGenerator->generateShips(m_config);
    initShipPositionsForPlayer(player, warShips);
}

bool BattleSeaGame::initShipPositionsForPlayer(const Player player, const std::vector<WarShip>& ships)
{
    // TODO validate ships position at least under some macro definition (debug for ex.)
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
    GridData& gridData = m_playerGrids[oppositePlayerIndex];
    std::vector<WarShip>& ships = m_playerShips[oppositePlayerIndex];

    if (gridData[cell.x()][cell.y()] != CellState::Concealed)
    {
        return ShotError::RepeatedShot;
    }

    // TODO ShotError::OutOfGrid; raname and check according to config values
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
            setGridCellState(gridData, cell, CellState::Damaged);
        }
        else
        {
            // Mark the whole ship in destroyed
            for (const CellIndex& shipCell : ship.getOccupiedCells())
            {
                setGridCellState(gridData, cell, CellState::Destroyed);
            }

            // Wrap all surrounded cells of the ship in missed
            surroundDestroyedShip(gridData, ship);
        }
        playerSwitch = false; // continue shooting
        break;
    }

    if (playerSwitch)
    {
        setGridCellState(gridData, cell, CellState::Missed);
        m_currentPlayer = getOppositePlayer(m_currentPlayer);
    }

    return ShotError::Ok;
}

void BattleSeaGame::startGame(const Player initialPlayer)
{
    m_initialPlayer = initialPlayer;
    m_currentPlayer = initialPlayer;

    // Validations before actual game
    assert(getLocalPlayer() != Player::Invalid);
}

bool BattleSeaGame::isGameOver() const
{
    // TODO impl
    return false;
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

void BattleSeaGame::setLocalPlayer(Player player)
{
    m_localPlayer = player;
}

const GridData BattleSeaGame::getPlayerGridInfo(const Player player) const
{
    // Create a copy of grid data to make the ship data visible for local player only
    const int playerIndex = getIndexFromPlayer(player);

    if (getLocalPlayer() != player)
    {
        return m_playerGrids[playerIndex];
    }

    GridData gridDataCopy = m_playerGrids[playerIndex];
    const std::vector<WarShip>& warShips = m_playerShips[playerIndex];

    for (const WarShip& ship : warShips)
    {
        for (const CellIndex& cell : ship.getOccupiedCells())
        {
            const auto& [x, y] = cell.asIndexesPair();
            if (gridDataCopy[x][y] == CellState::Concealed)
            {
                gridDataCopy[x][y] = CellState::Ship;
            }
        }
    }

    return gridDataCopy;
}

CellState BattleSeaGame::getPlayerGridCellState(const Player player, const CellIndex& cell) const
{
    return getPlayerGridInfo(player)[cell.x()][cell.y()];
}

int BattleSeaGame::getIndexFromPlayer(const Player& player)
{
    static_assert(static_cast<int>(Player::Player1) == 1);
    static_assert(static_cast<int>(Player::Player2) == 2);
    return static_cast<int>(player) - 1; // Excluding Invalid value from start
}

void BattleSeaGame::setGridCellState(GridData& outGridData, const CellIndex& cell, const CellState& state)
{
    outGridData[cell.x()][cell.y()] = state;
}

void BattleSeaGame::surroundDestroyedShip(GridData& outGridData, const WarShip& ship)
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

            if (outGridData[newX][newY] == CellState::Concealed)
            {
                outGridData[newX][newY] = CellState::Missed;
            }
        }
    }
}
