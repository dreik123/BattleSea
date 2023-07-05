#pragma once
#include "Core/CoreTypes.h"
#include "Game/GameGrid.h"
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

struct GameStartSettings
{
    Player initialPlayer;
    Player localPlayer;

    std::vector<WarShip> shipsForPlayer1;
    std::vector<WarShip> shipsForPlayer2;
};

// Main functionality in the game in model context
// - initialization of two grids for players with specified ship positions
// - grid generation for player
// - own info about current player and validation turn order
// - shooting specified cell on the certain grid
// - game over check

// Interface for major functionality of Battle sea game
class IBattleSeaGame
{
public:
    virtual bool initShipPositionsForPlayer(const Player player, const std::vector<WarShip>& ships) = 0;
    // ShootThePlayerGridAt method contains logic of the shot by current player(starting from initial one).
    virtual ShotError shootThePlayerGridAt(const CellIndex& cell) = 0;

    virtual bool startGame(const GameStartSettings& settings) = 0;
    virtual bool isGameOver() const = 0;

    virtual Player getCurrentPlayer() const = 0;
    virtual Player getInitialPlayer() const = 0;
    virtual Player getLocalPlayer() const = 0;

    virtual const GameGrid getPlayerGridInfo(const Player player) const = 0;
    virtual CellState getPlayerGridCellState(const Player player, const CellIndex& cell) const = 0;

    virtual const GameConfig& getAppliedConfig() const = 0;
};
