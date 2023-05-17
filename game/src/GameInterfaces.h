#pragma once
#include "CoreTypes.h"
#include "GameConfig.h"
#include <vector>


// Model is shortly presented by 2 game grids (10x10) for 2 players

// Every cell can represent one the states
//  - Concealed cell, which wasn't shot yet
//  - Missed shot, shot has been done, but no ship part is there
//              - Guarantted empty zone around destroyed ship, but for no is okay to present like MissedShot state
//  - Hit cell  (Damaged), shot has been done and some part of ship is damaged
//  - Destroyed, one or more cells which describes destroyed ship

// Do we have any difference in presentation of hit damaged and destroyed cells? - In concole - not sure, with GUI - yes, 100%

// Model must validate order of turns.


struct IWarShip
{
    virtual std::vector<CellIndex> GetOccupiedCells() const = 0;
    virtual int GetOccupiedCellsAmount() const = 0; // aka ship type

    //virtual bool IsDamaged() const = 0; // optional
    virtual bool IsDestroyed() const = 0;

    virtual bool DoesOccupyTheCell(const CellIndex& cell) const = 0;
    virtual void ShootShipAtCell(const CellIndex& cell) = 0;
    virtual bool TryShootShipAtCell(const CellIndex& cell) = 0; // optional
};



struct IBattleSeaGameObserver
{
    // Notify methods
    virtual void OnShipDestroyed(const IWarShip& InShip) = 0;
    virtual void OnShipDamaged(const IWarShip& InShip, const CellIndex& cell) = 0;
    virtual void OnShotMissed(const CellIndex& cell) = 0;
    virtual void OnShotDone(const CellIndex& cell) = 0;
    virtual void OnPlayerSwitched(CurrentPlayer nextPlayer) = 0;

    virtual void OnGameStarted() = 0;
    virtual void OnGameFinished(CurrentPlayer winner) = 0;
};





// Main functionality in the game in model context
// - initialization of two grids for players with specified ship positions
// - grid generation for player
// - own info about current player and validation turn order
// - shooting specified cell on the certain grid
// - game over check

// Interface for major functionality of Battle sea game
struct IBattleSeaGame
{
    virtual void GenerateShipsForGrid(const int gridIndex, const GameConfig& params) = 0;
    virtual bool InitShipPositionsForGrid(const int gridIndex, const std::vector<IWarShip>& ships) = 0;
    // Returned boolean value can indicate the shot success
    virtual bool ShootTheGridAt(const int gridIndex, const CellIndex& cell) = 0;

    virtual bool IsGameOver() const = 0;

    virtual CurrentPlayer GetCurrentPlayer() const = 0;
    virtual CurrentPlayer SetInitialPlayer(CurrentPlayer player) const = 0;


    virtual const GridData& GetGridInfo() const = 0;
    virtual CellState GetGridCellState(const CellIndex& cell) const = 0;
};
