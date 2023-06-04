#pragma once
#include <array>


enum class CellState
{
    Concealed = 0,
    Missed,
    Damaged,
    Destroyed,
};


// README if we decide to customize grid size based on config, need to consider std::vector instead of static array
constexpr int GridRowCount = 10;
constexpr int GridColumnCount = 10;
using GridData = std::array<std::array<CellState, GridColumnCount>, GridRowCount>;


enum class EPlayer : int8_t
{
    Invalid = -1,
    Player_1 = 0,
    Player_2,
};

__forceinline EPlayer GetOppositePlayer(const EPlayer player)
{
    if (player == EPlayer::Invalid) return EPlayer::Invalid;
    return (player == EPlayer::Player_1) ? EPlayer::Player_2 : EPlayer::Player_1;
}
