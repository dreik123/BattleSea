#pragma once
#include <stdint.h>
#include <memory>

#include "Core/EventBus.h"


enum class GameState : uint8_t
{
	Unitialized = 0,
	StartScreen,
	//MainMenu, // unused for current implementation
	ShipsSetup,
	Battle,
	GameOver,
};

