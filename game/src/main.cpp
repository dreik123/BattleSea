// BattleSea.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "MVCFactory.h"



int main(int argc, char* argv[])
{
    // Classic MVC (will be redone in the next PRs)
    auto factory = FactoryInterface::GetFactory();
    auto game = factory->CreateGame(GameConfig());
    auto presenter = factory->CreatePresenter(game);
    auto controller = factory->CreateController(game, presenter);

    controller->RunGame();

    return 0;
}
