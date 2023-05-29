#include "Controllers.h"
#include "Game/GameInterfaces.h"
#include "Views.h"

GameController::GameController(std::shared_ptr<IBattleSeaGame>& InGame, std::shared_ptr<IBattleSeaView>& InView)
    : Game(InGame)
    , View(InView)
{
}

void GameController::RunGame()
{
    // Temporary
    View->RenderGame();
}
