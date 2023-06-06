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
    Game->GenerateShipsForPlayer(EPlayer::Player_1);
    Game->GenerateShipsForPlayer(EPlayer::Player_2);

    const EPlayer initialPlayer  = EPlayer::Player_1;
    Game->SetLocalPlayer(EPlayer::Player_1); // will be refactored with IPlayer
    // TODO DS Probably local player data must be part of StartGame() + generated ships as well
    Game->StartGame(initialPlayer); // Does StartGame cover main menu or is this actual game (shooting) start?

    //if (std::cin.bad()) // TODO check

    bool hasGameBeenInterrupted = false;
    while (!hasGameBeenInterrupted && !Game->IsGameOver())
    {
        // Shows grids before first turn
        View->RenderGame();

        std::cout << "Player " << ((Game->GetCurrentPlayer() == EPlayer::Player_1) ? "1" : "2") << " turns:" << std::endl;
        std::string user_input;
        std::cin >> user_input;

        if (user_input == "q" || user_input == "quit")
        {
            hasGameBeenInterrupted = true;
            continue;
        }

        // TODO validate input
        /*bool hit = */Game->ShootThePlayerGridAt(CellIndex(user_input));

        // Clear screen to refresh the grids in place
        system("cls");
    }
}
