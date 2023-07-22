#pragma once
#include "Core/CoreTypes.h"
#include "Game/WarShip.h"
#include "Game/GameState.h"

#include <string>


// TODO consider implementation of all event classes separately and put all includes here


// I don't want to create init-ctors for all these events and would like to rely on aggregate (named) initialization (C++20 feature)
namespace events
{
    struct GameStateChangedEvent final
    {
        const GameState oldState;
        const GameState newState;
    };

    struct StartScreenPassedEvent final
    {
    };

    struct GridGeneratedEvent final
    {
        const GameGrid playerGridToConfirm;
    };

    struct FullGridsSyncEvent final
    {
        const GameGrid firstGrid;
        const GameGrid secondGrid;
    };

    struct ShotMissedEvent final
    {
        const Player shootingPlayer;
        const CellIndex shot;
    };

    struct ShipDestroyedEvent final
    {
        const Player injuredPlayer;
        const WarShip ship;
        const std::set<CellIndex> surroundedCells;
    };

    struct ShipDamagedEvent final
    {
        const Player injuredPlayer;
        const CellIndex shot;
    };

    struct PlayerTurnsEvent final
    {
        const std::string playerName;
        const bool isLocalPlayer;
    };

    struct PlayerSwitchedEvent final
    {
        const Player previousPlayer;
        const Player nextPlayer;
    };

    struct LocalShotErrorEvent final
    {
        const ShotError errorType;
    };

    struct GameStartedEvent final
    {
        const Player initialPlayer;
    };

    struct GameFinishedEvent final
    {
        const Player winner;
        const Player loser;
    };

    struct GameOverEvent final
    {
        const std::string winnerName;
        const bool isLocalPlayer;
    };

    struct GameRestartRequestEvent final
    {
    };

    struct QuitGameRequestEvent final
    {
    };
}
