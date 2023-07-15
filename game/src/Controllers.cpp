#include "Controllers.h"
#include "Views.h"

#include "Game/GameInterfaces.h"
#include "Game/GridUtilities.h"
#include "Game/Player/RealPlayer.h"
#include "Game/Player/SillyRandomBot.h"
#include "Game/Player/AIPlayer.h"

#include <iostream>
#include <conio.h>


GameController::GameController(std::shared_ptr<IBattleSeaGame>& game, std::shared_ptr<IBattleSeaView>& view)
    : m_game(game)
    , m_view(view)
    , m_shipsGenerator(new WarShipGenerator())
{
}

void GameController::runGame()
{
    m_view->renderGreetingToPlayer();

    // README For now ships generation is performed here in controller,
    // but potentially it might be carried out to IPlayer interface.

    // Regenerate ships for player while space is pressed (until enter)
    std::vector<WarShip> playerShips;
    char playerChoice = 0;
    do
    {
        playerShips = m_shipsGenerator->generateShips(m_game->getAppliedConfig());
        const GameGrid gridToPresent = GridUtilities::convertShipsToGrid(playerShips);
        m_view->renderGeneratedShips(gridToPresent);
        m_view->renderMessage("Do you like this setup?\nEnter - approve! Any button - regenerate\n");

        playerChoice = _getch();
    } while (playerChoice != '\r' && playerChoice != '\n');

    system("cls");

    // Game init
    m_players[0].reset(new RealPlayer(Player::Player1));
    //m_players[0].reset(new SillyBotPlayer(Player::Player1, m_game)); // can be useful
    // TODO AP please replace it with advanced AI bot when it's implemented
    m_players[1].reset(new AIPlayer(Player::Player2, m_game));

    GameStartSettings settings;
    settings.initialPlayer = Player::Player1;
    settings.localPlayer = Player::Player1;
    settings.shipsForPlayer1 = playerShips;
    settings.shipsForPlayer2 = m_shipsGenerator->generateShips(m_game->getAppliedConfig());

    if (!m_game->startGame(settings))
    {
        std::cerr << "Invalid settings for game start!\n";
        return;
    }

    // Shows grids before first turn
    m_view->renderGame();

    bool hasGameBeenInterrupted = false;
    while (!hasGameBeenInterrupted && !m_game->isGameOver())
    {
        IPlayer& currentPlayer = getCurrentPlayer(m_game->getCurrentPlayer());
        m_view->renderRequestToTurn(currentPlayer.getName());

        bool isValidTurn = false;
        do
        {
            const InputRequest userInput = currentPlayer.getInput();

            if (userInput.isQuitRequested)
            {
                hasGameBeenInterrupted = true;
                break;
            }

            if (userInput.shotCell.has_value())
            {
                const ShotError result = m_game->shootThePlayerGridAt(userInput.shotCell.value());

                m_view->renderShotError(result);
                isValidTurn = result == ShotError::Ok;
            }
            else
            {
                m_view->renderMessage("WTF have you entered?!?\n");
            }
        } while (!isValidTurn);

        // Clear screen to refresh the grids in place
        system("cls");

        m_view->renderGame();
    }

    if (m_game->isGameOver())
    {
        // The current player hasn't been changed after last shot.
        IPlayer& winner = getCurrentPlayer(m_game->getCurrentPlayer());
        m_view->renderGameOver(winner.getName(), winner.isLocalPlayer());
        m_view->renderMessage("Please relaunch game if you want to play again\n");
    }
}

IPlayer& GameController::getCurrentPlayer(const Player player) const
{
    const int index = getIndexFromPlayer(player);
    return *m_players[index];
}
