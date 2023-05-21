// BattleSea.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "GameInterfaces.h"
#include "MVCFactory.h"


class GameController : public IController {};


int main(int argc, char* argv[])
{
    // Classic MVC
    IGame* game = CreateGame(GameConfig());
    IView* presenter = CreatePresenter();
    IController* controller = CreateController(game, presenter);

    return 0;
}
