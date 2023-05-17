#pragma once
#include <stdint.h>


struct GameConfig
{
    uint8_t AmountOfShipWithSixHoles;
    uint8_t AmountOfShipWithFiveHoles;
    uint8_t AmountOfShipWithFourHoles;
    uint8_t AmountOfShipWithThreeHoles;
    uint8_t AmountOfShipWithTwoHoles;
    uint8_t AmountOfShipWithOneHoles;

    uint8_t sizeX = 10;
    uint8_t sizeY = 10;
};

// TODO DS make this creation a bit user friendly
static GameConfig DefaultGameConfig = GameConfig{ 0, 0, 1, 2, 3, 4 };
static GameConfig HasbroGameConfig = GameConfig{ 0, 1, 1, 1, 1, 1 };
