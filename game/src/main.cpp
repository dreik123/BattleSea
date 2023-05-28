// BattleSea.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "MVCFactory.h"



int main(int argc, char* argv[])
{
    // Classic MVC (will be redone in the next PRs)
    auto game = CreateGame(GameConfig());
    auto presenter = CreatePresenter(game);
    auto controller = CreateController(game, presenter);

    controller->RunGame();

    return 0;
}
