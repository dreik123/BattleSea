#include "Controllers.h"
#include "Game/GameInterfaces.h"
#include "Views.h"
#include "Game/Player/RealPlayer.h"
#include "Game/Player/SillyRandomBot.h"

#include <iostream>
#include <string>

// TODOs:
// introduce Iplayer and encapsulate turn logic there
// create RealPlayer
// create seconds player as AIPlayer

// impl CellIndex::fromString(..) instead of ctor
// all entities with logic must be classes, if class contains data only, make it a struct

GameController::GameController(std::shared_ptr<IBattleSeaGame>& game, std::shared_ptr<IBattleSeaView>& view)
    : m_game(game)
    , m_view(view)
{
}

void GameController::runGame()
{
    m_players[Player::Player1] = new RealPlayer(Player::Player1);
    m_players[Player::Player2] = new SillyBotPlayer(Player::Player2, m_game);

    // [Temporary] TODO DS Player can regenerate ships many times before game start
    m_game->generateShipsForPlayer(Player::Player1);
    m_game->generateShipsForPlayer(Player::Player2);

    const Player initialPlayer = Player::Player1;
    m_game->setLocalPlayer(Player::Player1);
    // TODO DS Probably local player data must be part of startGame() + generated ships as well
    m_game->startGame(initialPlayer); // Does startGame cover main menu or is this actual game (shooting) start?

    //if (std::cin.bad()) // TODO check

    bool hasGameBeenInterrupted = false;
    while (!hasGameBeenInterrupted && !m_game->isGameOver())
    {
        IPlayer* currentPlayer = m_players.at(m_game->getCurrentPlayer());

        // Shows grids before first turn
        m_view->renderGame();

        std::cout << currentPlayer->getName() << " turns:" << std::endl;
        std::optional<InputRequest> userInputOpt = currentPlayer->getInput();
        if (!userInputOpt.has_value())
        {
            // TODO process properly - reask for the input
            std::cout << "WTF?? Exiting...\n";
            hasGameBeenInterrupted = true;
            continue;
        }

        const InputRequest userInput = userInputOpt.value();

        if (userInput.isQuitRequested)
        {
            hasGameBeenInterrupted = true;
            continue;
        }

        if (userInput.shotCell.has_value())
        {
            // TODO validate input
            m_game->shootThePlayerGridAt(userInput.shotCell.value());
        }


        // Clear screen to refresh the grids in place
        system("cls");
    }
}
