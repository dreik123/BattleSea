#pragma once
#include <array>

using CellIndex = std::pair<char, char>;

enum class CellState
{
    Concealed = 0,
    Missed,
    Damaged,
    Destroyed,
};



// README if we decide to customize grid size based on config, need to consider std::vector instead of static array
using GridData = std::array<std::array<CellState, 10>, 10>;


enum class CurrentPlayer : int8_t
{
    Invalid = -1,
    Player_1 = 0,
    Player_2,
};
