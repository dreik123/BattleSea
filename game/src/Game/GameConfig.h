#pragma once
#include <stdint.h>
#include <set>

struct GameConfig
{
    std::multiset<uint8_t> numberOfMultiDeckShips;

    uint8_t sizeX = 10;
    uint8_t sizeY = 10;
};

// TODO DS make this creation a bit user friendly
static const GameConfig DEFAULT_GAME_CONFIG = GameConfig{ std::multiset<uint8_t>{ 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 } };
static const GameConfig HASBRO_GAME_CONFIG = GameConfig{ std::multiset<uint8_t>{ 5, 4, 3, 2, 1 } };
