#include "BattleSeaGame.h"
#include "Game/WarShipGenerators.h"
#include "Game/WarShip.h"


BattleSeaGame::BattleSeaGame(std::unique_ptr<IWarShipGenerator>&& InGenerator, const GameConfig& InConfig)
    : GridGenerator(std::move(InGenerator))
    , Config(InConfig)
    , PlayerGrids{}
    , PlayerShips{}
    , CurrentPlayer(EPlayer::Invalid)
    , InitialPlayer(EPlayer::Invalid)
    , LocalPlayer(EPlayer::Invalid)
{
    for (GridData& gridData : PlayerGrids)
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

void BattleSeaGame::GenerateShipsForPlayer(const EPlayer player)
{
    const std::vector<WarShip> warShips = GridGenerator->GenerateShips(Config);
    InitShipPositionsForPlayer(player, warShips);
}

bool BattleSeaGame::InitShipPositionsForPlayer(const EPlayer player, const std::vector<WarShip>& ships)
{
    // TODO validate ships position at least under some macro definition (debug for ex.)
    const bool isValidated = true;
    if (!isValidated)
    {
        return false;
    }

    PlayerShips[GetIndexFromPlayer(player)] = ships;

    return true;
}

bool BattleSeaGame::ShootThePlayerGridAt(const CellIndex& cell)
{
    assert(GetCurrentPlayer() != EPlayer::Invalid);

    const int oppositePlayerIndex = GetIndexFromPlayer(GetOppositePlayer(GetCurrentPlayer()));
    GridData& gridData = PlayerGrids[oppositePlayerIndex];
    std::vector<WarShip>& ships = PlayerShips[oppositePlayerIndex];

    bool playerSwitch = true;

    // FIXME it corrupts damaged and destroyed states
    SetGridCellState(gridData, cell, CellState::Missed);
    for (WarShip& ship : ships)
    {
        if (ship.IsDestroyed() || !ship.DoesOccupyTheCell(cell))
        {
            continue;
        }

        ship.ShootShipAtCell(cell);

        if (!ship.IsDestroyed())
        {
            SetGridCellState(gridData, cell, CellState::Damaged);
        }
        else
        {
            // Mark the whole ship in destroyed
            for (const CellIndex& shipCell : ship.GetOccupiedCells())
            {
                SetGridCellState(gridData, cell, CellState::Destroyed);
            }

            // Wrap all surrounded cells of the ship in missed
            SurroundDestroyedShip(gridData, ship);
        }
        playerSwitch = false; // continue shooting
        break;
    }

    if (playerSwitch)
    {
        CurrentPlayer = GetOppositePlayer(CurrentPlayer);
    }
    return !playerSwitch;
}

void BattleSeaGame::StartGame(const EPlayer initialPlayer)
{
    InitialPlayer = initialPlayer;
    CurrentPlayer = initialPlayer;

    // Validations before actual game
    assert(GetLocalPlayer() != EPlayer::Invalid);
}

bool BattleSeaGame::IsGameOver() const
{
    // TODO impl
    return false;
}

EPlayer BattleSeaGame::GetCurrentPlayer() const
{
    return CurrentPlayer;
}

EPlayer BattleSeaGame::GetInitialPlayer() const
{
    return InitialPlayer;
}

EPlayer BattleSeaGame::GetLocalPlayer() const
{
    return LocalPlayer;
}

void BattleSeaGame::SetLocalPlayer(EPlayer player)
{
    LocalPlayer = player;
}

const GridData BattleSeaGame::GetPlayerGridInfo(const EPlayer player) const
{
    // Create a copy of grid data to make the ship data visible for local player only
    const int playerIndex = GetIndexFromPlayer(player);
    GridData gridDataCopy = PlayerGrids[playerIndex];
    const std::vector<WarShip>& warShips = PlayerShips[playerIndex];

    if (GetLocalPlayer() == player)
    {
        for (const WarShip& ship : warShips)
        {
            for (const CellIndex& cell : ship.GetOccupiedCells())
            {
                const auto& [x, y] = cell.AsIndexesPair();
                if (gridDataCopy[x][y] == CellState::Concealed)
                {
                    gridDataCopy[x][y] = CellState::Ship;
                }
            }
        }
    }
    return gridDataCopy;
}

CellState BattleSeaGame::GetPlayerGridCellState(const EPlayer player, const CellIndex& cell) const
{
    return GetPlayerGridInfo(player)[cell.X()][cell.Y()];
}

int BattleSeaGame::GetIndexFromPlayer(const EPlayer& player)
{
    return static_cast<int>(player);
}

void BattleSeaGame::SetGridCellState(GridData& outGridData, const CellIndex& cell, const CellState& state)
{
    outGridData[cell.X()][cell.Y()] = state;
}

void BattleSeaGame::SurroundDestroyedShip(GridData& outGridData, const WarShip& ship)
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
    for (const CellIndex& cell : ship.GetOccupiedCells())
    {
        const auto& [x, y] = cell.AsIndexesPair();
        for (const auto& [dX, dY] : kPossibleDirections)
        {
            const int newX = x + dX;
            const int newY = y + dY;
            if (newX < 0 || newY < 0 || newX >= GridRowCount || newY >= GridColumnCount)
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
