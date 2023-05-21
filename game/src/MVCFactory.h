#pragma once
#include "MVCInterfaces.h"
#include "GameConfig.h"

// TODO declare a factory class with these methods
IController* CreateController(IGame* game, IView* presenter) { return nullptr; }

IGame* CreateGame(const GameConfig&) { return nullptr; }

IView* CreatePresenter() { return new TerminalPresenter(); }
