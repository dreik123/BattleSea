// BattleSea.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "MVCFactory.h"
#include "Core/EventBus.h"


int main(int argc, char* argv[])
{
    // Classic MVC (will be redone in the next PRs)
    auto eventBus = std::make_shared<EventBus>();

    auto factory = FactoryInterface::getFactory();
    auto game = factory->createGame(eventBus);
    auto presenter = factory->createPresenter(game, eventBus);
    auto controller = factory->createController(game, presenter, eventBus);

    controller->runGame();

    return 0;
}
