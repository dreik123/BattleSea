// BattleSea.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "MVCFactory.h"


int main(int argc, char* argv[])
{
    // Classic MVC (will be redone in the next PRs)
    auto factory = FactoryInterface::getFactory();
    auto game = factory->createGame();
    auto presenter = factory->createPresenter(game);
    auto controller = factory->createController(game, presenter);

    controller->runGame();

    return 0;
}
