#include "Controllers.h"
#include "Game/GameInterfaces.h"
#include "Views.h"

#include <iostream>
#include <string>

// TODOs:
// introduce Iplayer and encapsulate turn logic there
// create RealPlayer
// create seconds player as AIPlayer

GameController::GameController(std::shared_ptr<IBattleSeaGame>& InGame, std::shared_ptr<IBattleSeaView>& InView)
    : Game(InGame)
    , View(InView)
{
}

void GameController::RunGame()
{
    // [Temporary] TODO DS Player can regenerate ships many times before game start
    Game->generateShipsForPlayer(EPlayer::Player_1);
    Game->generateShipsForPlayer(EPlayer::Player_2);

    const EPlayer initialPlayer  = EPlayer::Player_1;
    Game->setLocalPlayer(EPlayer::Player_1); // will be refactored with IPlayer
    // TODO DS Probably local player data must be part of startGame() + generated ships as well
    Game->startGame(initialPlayer); // Does startGame cover main menu or is this actual game (shooting) start?

    //if (std::cin.bad()) // TODO check

    bool hasGameBeenInterrupted = false;
    while (!hasGameBeenInterrupted && !Game->isGameOver())
    {
        // Shows grids before first turn
        View->RenderGame();

        std::cout << "Player " << ((Game->getCurrentPlayer() == EPlayer::Player_1) ? "1" : "2") << " turns:" << std::endl;
        std::string user_input;
        std::cin >> user_input;

        if (user_input == "q" || user_input == "quit")
        {
            hasGameBeenInterrupted = true;
            continue;
        }

        // Temp debug approach
        if (user_input == "g")
        {
            Game->generateShipsForPlayer(EPlayer::Player_1);
            system("cls");
            continue;
        }

        // TODO validate input
        /*bool hit = */Game->shootThePlayerGridAt(CellIndex(user_input));

        // Clear screen to refresh the grids in place
        system("cls");
    }
}
