#pragma once
#include <stdint.h>
#include <set>

struct GameConfig
{
    std::multiset<uint8_t> numberOfMultiDeckShips;

    uint8_t rowsCount;
    uint8_t columnsCount;
};


constexpr int CLASSIC_GRID_ROW_COUNT = 10;
constexpr int CLASSIC_GRID_COLUMN_COUNT = 10;


template<size_t N>
constexpr std::array<char, N> generateNCharSequence()
{
    std::array<char, N> result;
    for (int i = 0; i < N; i++)
    {
        result[i] = 'A' + i;
    }
    return result;
}

constexpr static std::array<char, 10> TestSequence = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' };
static_assert(generateNCharSequence<10>() == TestSequence);

constexpr static std::array<char, 10> ROW_AXIS_NAMES = generateNCharSequence<10>();


static const GameConfig DEFAULT_GAME_CONFIG = GameConfig
{
    std::multiset<uint8_t>{ 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 },
    CLASSIC_GRID_ROW_COUNT,
    CLASSIC_GRID_COLUMN_COUNT,
};


static const GameConfig HASBRO_GAME_CONFIG = GameConfig
{
    std::multiset<uint8_t>{ 5, 4, 3, 2, 1 },
    CLASSIC_GRID_ROW_COUNT,
    CLASSIC_GRID_COLUMN_COUNT,
};

// TODO AP experiment with possible variations
