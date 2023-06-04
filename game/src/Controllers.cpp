#include "Controllers.h"
#include "Game/GameInterfaces.h"
#include "Views.h"

#include <iostream>
#include <string>

GameController::GameController(std::shared_ptr<IBattleSeaGame>& InGame, std::shared_ptr<IBattleSeaView>& InView)
    : Game(InGame)
    , View(InView)
{
}

void GameController::RunGame()
{
    // Temporary
    Game->GenerateShipsForPlayer(EPlayer::Player_1, DefaultGameConfig);
    Game->GenerateShipsForPlayer(EPlayer::Player_2, DefaultGameConfig);
    Game->SetInitialPlayer(EPlayer::Player_1);
    View->RenderGame();

    //if (std::cin.bad()) // TODO check

    bool hasGameBeenInterrupted = false;
    EPlayer currentPlayerHACK = EPlayer::Player_1;
    while (!hasGameBeenInterrupted && !Game->IsGameOver())
    {
        std::cout << "Player " << ((currentPlayerHACK == EPlayer::Player_1) ? "1" : "2") << " turns:" << std::endl;
        std::string user_input;
        std::cin >> user_input;

        if (user_input == "q" || user_input == "quit")
        {
            hasGameBeenInterrupted = true;
            continue;
        }

        // TODO validate
        bool hit = Game->ShootThePlayerGridAt(CellIndex(user_input));
        if (!hit)
        {
            // Temp HACK: since no hit has been done, player switched
            currentPlayerHACK = GetOppositePlayer(currentPlayerHACK);
        }
        View->RenderGame();
    }
}
