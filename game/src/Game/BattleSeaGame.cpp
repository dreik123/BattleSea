#include "BattleSeaGame.h"
#include "Game/WarShipGenerators.h"
#include "Game/WarShip.h"


BattleSeaGame::BattleSeaGame(std::unique_ptr<IWarShipGenerator>&& generator, const GameConfig& config)
    : m_gridGenerator(std::move(generator))
    , m_config(config)
    , m_playerGrids{}
    , m_playerShips{}
    , m_currentPlayer(EPlayer::Invalid)
    , m_initialPlayer(EPlayer::Invalid)
    , m_localPlayer(EPlayer::Invalid)
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

void BattleSeaGame::generateShipsForPlayer(const EPlayer player)
{
    const std::vector<WarShip> warShips = m_gridGenerator->generateShips(m_config);
    initShipPositionsForPlayer(player, warShips);
}

bool BattleSeaGame::initShipPositionsForPlayer(const EPlayer player, const std::vector<WarShip>& ships)
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

bool BattleSeaGame::shootThePlayerGridAt(const CellIndex& cell)
{
    assert(getCurrentPlayer() != EPlayer::Invalid);

    const int oppositePlayerIndex = getIndexFromPlayer(GetOppositePlayer(getCurrentPlayer()));
    GridData& gridData = m_playerGrids[oppositePlayerIndex];
    std::vector<WarShip>& ships = m_playerShips[oppositePlayerIndex];

    // TODO need to properly validate the double shot at cell outside
    assert(gridData[cell.x()][cell.y()] == CellState::Concealed && "Double shot");

    bool playerSwitch = true;

    // FIXME it corrupts damaged and destroyed states
    setGridCellState(gridData, cell, CellState::Missed);
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
        m_currentPlayer = GetOppositePlayer(m_currentPlayer);
    }
    return !playerSwitch;
}

void BattleSeaGame::startGame(const EPlayer initialPlayer)
{
    m_initialPlayer = initialPlayer;
    m_currentPlayer = initialPlayer;

    // Validations before actual game
    assert(getLocalPlayer() != EPlayer::Invalid);
}

bool BattleSeaGame::isGameOver() const
{
    // TODO impl
    return false;
}

EPlayer BattleSeaGame::getCurrentPlayer() const
{
    return m_currentPlayer;
}

EPlayer BattleSeaGame::getInitialPlayer() const
{
    return m_initialPlayer;
}

EPlayer BattleSeaGame::getLocalPlayer() const
{
    return m_localPlayer;
}

void BattleSeaGame::setLocalPlayer(EPlayer player)
{
    m_localPlayer = player;
}

const GridData BattleSeaGame::getPlayerGridInfo(const EPlayer player) const
{
    // Create a copy of grid data to make the ship data visible for local player only
    const int playerIndex = getIndexFromPlayer(player);
    GridData gridDataCopy = m_playerGrids[playerIndex];
    const std::vector<WarShip>& warShips = m_playerShips[playerIndex];

    if (getLocalPlayer() == player)
    {
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
    }
    return gridDataCopy;
}

CellState BattleSeaGame::getPlayerGridCellState(const EPlayer player, const CellIndex& cell) const
{
    return getPlayerGridInfo(player)[cell.x()][cell.y()];
}

int BattleSeaGame::getIndexFromPlayer(const EPlayer& player)
{
    static_assert(static_cast<int>(EPlayer::Player_1) == 1);
    static_assert(static_cast<int>(EPlayer::Player_2) == 2);
    return static_cast<int>(player) - 1; // Excluding Invalid value from start
}

void BattleSeaGame::setGridCellState(GridData& outGridData, const CellIndex& cell, const CellState& state)
{
    outGridData[cell.x()][cell.y()] = state;
}

void BattleSeaGame::surroundDestroyedShip(GridData& outGridData, const WarShip& ship)
{
    // Example of class template auto deduction (CTAD)
    constexpr std::array kPossibleDirections =
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
        for (const auto& [dX, dY] : kPossibleDirections)
        {
            const int newX = x + dX;
            const int newY = y + dY;
            if (newX < 0 || newY < 0 || newX >= GRID_ROW_COUNT || newY >= GRID_COLUMN_COUNT)
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
