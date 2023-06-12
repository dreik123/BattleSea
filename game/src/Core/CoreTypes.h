#pragma once
#include <array>


enum class CellState
{
    Concealed = 0,      // No shot has been done to the cell
    Ship,               // Ship presentation on grid (for local player only)
    Missed,             // Shot has been done, but no ship part damaged
    Damaged,            // Shot right on target, but ship is NOT destroyed
    Destroyed,          // Shot right on target, but ship is destroyed
};


// README if we decide to customize grid size based on config, need to consider std::vector instead of static array
constexpr int GRID_ROW_COUNT = 10;
constexpr int GRID_COLUMN_COUNT = 10;
using GridData = std::array<std::array<CellState, GRID_COLUMN_COUNT>, GRID_ROW_COUNT>;


enum class EPlayer : int8_t
{
    Invalid = 0,
    Player1,
    Player2,

    Count = Player2 + 1, // to cover starting count from zero
};

static_assert(static_cast<int>(EPlayer::Count) == 3);

__forceinline EPlayer getOppositePlayer(const EPlayer player)
{
    if (player == EPlayer::Invalid) return EPlayer::Invalid;
    return (player == EPlayer::Player1) ? EPlayer::Player2 : EPlayer::Player1;
}
