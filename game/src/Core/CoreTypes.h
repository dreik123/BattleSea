#pragma once
#include <stdint.h>

enum class CellState : uint8_t
{
    Concealed = 0,      // No shot has been done to the cell
    Ship,               // Ship presentation on grid (for local player only)
    Missed,             // Shot has been done, but no ship part damaged
    Damaged,            // Shot right on target, but ship is NOT destroyed
    Destroyed,          // Shot right on target, but ship is destroyed
};

enum class ShotError : uint8_t
{
    Ok = 0,
    OutOfGrid,
    RepeatedShot,
};


enum class Player : uint8_t
{
    Invalid = 0,
    Player1,
    Player2,

    Count = Player2 + 1, // to cover starting count from zero
};

static_assert(static_cast<int>(Player::Count) == 3);

__forceinline Player getOppositePlayer(const Player player)
{
    if (player == Player::Invalid) return Player::Invalid;
    return (player == Player::Player1) ? Player::Player2 : Player::Player1;
}

__forceinline int getIndexFromPlayer(const Player& player)
{
    static_assert(static_cast<int>(Player::Player1) == 1);
    static_assert(static_cast<int>(Player::Player2) == 2);
    return static_cast<int>(player) - 1; // Excluding Invalid value from start
}
