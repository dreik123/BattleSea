#include "BattleSeaGame.h"
#include "Game/WarShipGenerators.h"
#include "Game/WarShip.h"

BattleSeaGame::BattleSeaGame()
{
    //auto generator = std::make_unique<PredefinedClassicWarShipGenerator>();
    //SetWarShipGenerator(std::move(generator));
    GridGenerator = std::make_unique<PredefinedClassicWarShipGenerator>();
    CurrentPlayer = EPlayer::Player_1;

    // TODO DS impl PlayerGrids initialization
}

//void BattleSeaGame::SetWarShipGenerator(const std::unique_ptr<IWarShipGenerator>& generator)
//{
//}

void BattleSeaGame::GenerateShipsForPlayer(const EPlayer player, const GameConfig& params)
{
    const std::vector<WarShip> warShips = GridGenerator->GenerateShips(params);
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

bool BattleSeaGame::ShootThePlayerGridAt(const EPlayer player, const CellIndex& cell)
{
    GridData& gridData = PlayerGrids[GetIndexFromPlayer(player)];
    auto& ships = PlayerShips[GetIndexFromPlayer(player)];

    bool playerSwitch = true;

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
        CurrentPlayer = GetNextPlayer();
    }
}

bool BattleSeaGame::IsGameOver() const
{
    return false;
}

EPlayer BattleSeaGame::GetCurrentPlayer() const
{
    return CurrentPlayer;
}

void BattleSeaGame::SetInitialPlayer(EPlayer player)
{
    CurrentPlayer = player;
}

const GridData& BattleSeaGame::GetPlayerGridInfo(const EPlayer player) const
{
    return PlayerGrids[GetIndexFromPlayer(player)];
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

EPlayer BattleSeaGame::GetNextPlayer() const
{
    assert(CurrentPlayer != EPlayer::Invalid);

    return (CurrentPlayer == EPlayer::Player_1) ? EPlayer::Player_2 : EPlayer::Player_1;
}
