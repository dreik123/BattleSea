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
constexpr int GridRowCount = 10;
constexpr int GridColumnCount = 10;
using GridData = std::array<std::array<CellState, GridColumnCount>, GridRowCount>;


enum class EPlayer : int8_t
{
    Invalid = 0,
    Player_1,
    Player_2,

    Count = Player_2 + 1, // to cover starting count from zero
};

static_assert(static_cast<int>(EPlayer::Count) == 3);

__forceinline EPlayer GetOppositePlayer(const EPlayer player)
{
    if (player == EPlayer::Invalid) return EPlayer::Invalid;
    return (player == EPlayer::Player_1) ? EPlayer::Player_2 : EPlayer::Player_1;
}
