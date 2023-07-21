// BattleSea.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "GameFactory.h"
#include "Core/EventBus.h"

// Known improvements for the game, but no time to implement right now:
// 
// EventBus publish() call triggers another publish() calls during listeners execution|  (event bus update in separate thread as option).
// Input from user should be abstraction, but probably controller should process it instead of player (Should have to wrap common interface over Terminal and AI players).
// Current TerminalView implementation can't render single shot and refresh entire grids.
//      (I'd rather introduce graphical library than will fix it in console).
// Talking about some graphics it makes sense to try SDL or SFML in the game | Graphics Controller and View should be implemented then.
// In-place assertations should be moved to 'tests' project.


int main(int argc, char* argv[])
{
    do
    {
        auto eventBus = std::make_shared<EventBus>();

        auto factory = FactoryInterface::getFactory<ClassicTerminalBattleSeaFactory>();
        auto game = factory->createGame(eventBus);
        auto controller = factory->createController(std::move(game), eventBus);

        controller->loopGame();
        if (!controller->hasRestartRequested())
        {
            break;
        }
    } while (true);

    return 0;
}
