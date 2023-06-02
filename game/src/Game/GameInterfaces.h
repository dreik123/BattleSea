#pragma once
#include "Core/CoreTypes.h"
#include "Game/GameConfig.h"
#include "Game/WarShipGenerators.h"
#include "Game/WarShip.h"

#include <vector>
#include <memory>


// Model is shortly presented by 2 game grids (10x10) for 2 players

// Every cell can represent one the states
//  - Concealed cell, which wasn't shot yet
//  - Missed shot, shot has been done, but no ship part is there
//              - Guarantted empty zone around destroyed ship, but for no is okay to present like MissedShot state
//  - Hit cell  (Damaged), shot has been done and some part of ship is damaged
//  - Destroyed, one or more cells which describes destroyed ship

// Do we have any difference in presentation of hit damaged and destroyed cells? - In concole - not sure, with GUI - yes, 100%

// Model must validate order of turns.


struct IBattleSeaGameObserver
{
    // Notify methods
    virtual void OnShipDestroyed(const WarShip& InShip) = 0;
    virtual void OnShipDamaged(const WarShip& InShip, const CellIndex& cell) = 0;
    virtual void OnShotMissed(const CellIndex& cell) = 0;
    virtual void OnShotDone(const CellIndex& cell) = 0;
    virtual void OnPlayerSwitched(EPlayer nextPlayer) = 0;

    virtual void OnGameStarted() = 0;
    virtual void OnGameFinished(EPlayer winner) = 0;
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
    //virtual void SetWarShipGenerator(const std::unique_ptr<IWarShipGenerator>& generator) = 0;

    virtual void GenerateShipsForPlayer(const EPlayer player, const GameConfig& params) = 0;
    virtual bool InitShipPositionsForPlayer(const EPlayer player, const std::vector<WarShip>& ships) = 0;
    // Returned boolean value can indicate the shot success
    virtual bool ShootThePlayerGridAt(const EPlayer player, const CellIndex& cell) = 0;

    virtual bool IsGameOver() const = 0;

    virtual EPlayer GetCurrentPlayer() const = 0;
    virtual void SetInitialPlayer(EPlayer player) = 0;


    virtual const GridData& GetPlayerGridInfo(const EPlayer player) const = 0;
    virtual CellState GetPlayerGridCellState(const EPlayer player, const CellIndex& cell) const = 0;
};
