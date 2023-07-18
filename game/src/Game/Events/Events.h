#pragma once
#include "Core/CoreTypes.h"
#include "Core/CoreDefines.h"
#include "Core/Event.h"
#include "Game/WarShip.h"
#include "Game/GameState.h"

#include <string>


// TODO consider implementation of all event classes separately and put all includes here


// I don't want to create init-ctors for all these events and would like to rely on aggregate (named) initialization
namespace events
{
    struct GameStateChangedEvent final
#if !EVENTS_STD_ANY_APPROACH
        : public Event
#endif
    {
        GameState oldState;
        GameState newState;
    };

    struct StartScreenPassedEvent final
#if !EVENTS_STD_ANY_APPROACH
        : public Event
#endif
    {
    };

    struct GridGeneratedEvent final
#if !EVENTS_STD_ANY_APPROACH
        : public Event
#endif
    {
        GameGrid playerGridToConfirm;
    };

    struct FullGridsSyncEvent final
#if !EVENTS_STD_ANY_APPROACH
        : public Event
#endif
    {
        GameGrid firstGrid;
        GameGrid secondGrid;
    };

    /////////////////////////////////
    struct ShotMissedEvent final
#if !EVENTS_STD_ANY_APPROACH
        : public Event
#endif
    {
        const Player shootingPlayer;
        const CellIndex shot;
    };

    struct ShipDestroyedEvent final
#if !EVENTS_STD_ANY_APPROACH
        : public Event
#endif
    {
        const Player injuredPlayer;
        const WarShip ship;
    };

    struct ShipDamagedEvent final
#if !EVENTS_STD_ANY_APPROACH
        : public Event
#endif
    {
        const Player injuredPlayer;
        const WarShip ship;
        const CellIndex shot;
    };

    struct PlayerTurnsEvent final
#if !EVENTS_STD_ANY_APPROACH
        : public Event
#endif
    {
        const std::string playerName;
        const bool isLocalPlayer;
    };

    struct PlayerSwitchedEvent final
#if !EVENTS_STD_ANY_APPROACH
        : public Event
#endif
    {
        const Player previousPlayer;
        const Player nextPlayer;
    };

    struct LocalShotErrorEvent final
#if !EVENTS_STD_ANY_APPROACH
        : public Event
#endif
    {
        const ShotError errorType;
    };

    struct GameStartedEvent final
#if !EVENTS_STD_ANY_APPROACH
        : public Event
#endif
    {
        const Player initialPlayer;
    };

    struct GameFinishedEvent final
#if !EVENTS_STD_ANY_APPROACH
        : public Event
#endif
    {
        const Player winner;
        const Player loser;
    };

    struct GameOverEvent final
#if !EVENTS_STD_ANY_APPROACH
        : public Event
#endif
    {
        const std::string winnerName;
        const bool isLocalPlayer;
    };

    struct QuitGameRequestEvent final
#if !EVENTS_STD_ANY_APPROACH
        : public Event
#endif
    {
    };
}
