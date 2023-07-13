// BattleSea.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "GameFactory.h"
#include "Core/EventBus.h"


int main(int argc, char* argv[])
{
    auto eventBus = std::make_shared<EventBus>();

    auto factory = FactoryInterface::getFactory<ClassicTerminalBattleSeaFactory>();
    auto game = factory->createGame(eventBus);
    auto controller = factory->createController(std::move(game), eventBus);

    controller->loopGame();

    return 0;
}
