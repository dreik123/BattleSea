#include "Controllers.h"
#include "Views.h"

#include "Game/GameInterfaces.h"
#include "Game/GridUtilities.h"
#include "Game/Player/RealPlayer.h"
#include "Game/Player/SillyRandomBot.h"

#include <iostream>
#include <conio.h>

// TODO review std::cout calls in the file and consider to move them to view

GameController::GameController(std::shared_ptr<IBattleSeaGame>& game, std::shared_ptr<IBattleSeaView>& view)
    : m_game(game)
    , m_view(view)
    , m_shipsGenerator(new WarShipGenerator())
{
}

void GameController::runGame()
{
    std::cout << "Welcome to Battle Sea game. Enjoy it.\nPress any key to choose your setup\n";
    int _ = _getch();

    // README For now ships generation is performed here in controller,
    // but potentially it might be carried out to IPlayer interface.

    // Regenerate ships for player while space is pressed (until enter)
    std::vector<WarShip> playerShips;
    char playerChoice = 0;
    do
    {
        system("cls");
        std::cout << "Generated the ships for you:\n";
        playerShips = m_shipsGenerator->generateShips(m_game->getAppliedConfig());
        const GameGrid gridToPresent = GridUtilities::convertShipsToGrid(playerShips);
        m_view->renderGeneratedShips(gridToPresent);
        std::cout << "Do you like this setup?\nEnter - approve! Any button - regenerate\n";

        playerChoice = _getch();
    } while (playerChoice != '\r' && playerChoice != '\n');

    system("cls");

    // Game init
    m_players[0].reset(new RealPlayer(Player::Player1));
    // TODO AP please replace it with advanced AI bot when it's implemented
    m_players[1].reset(new SillyBotPlayer(Player::Player2, m_game));

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

    //if (std::cin.bad()) // TODO check

    // Shows grids before first turn
    m_view->renderGame();

    bool hasGameBeenInterrupted = false;
    while (!hasGameBeenInterrupted && !m_game->isGameOver())
    {
        IPlayer& currentPlayer = getCurrentPlayer(m_game->getCurrentPlayer());

        std::cout << currentPlayer.getName() << " turns:" << std::endl;

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
                switch (result)
                {
                case ShotError::Ok:
                    isValidTurn = true;
                    break;
                case ShotError::OutOfGrid:
                    std::cout << "Out of grid. Try again\n";
                    break;
                case ShotError::RepeatedShot:
                    std::cout << "You've already shooted at this cell! Try again\n";
                    break;
                default:
                    assert(false && "Unexpected ShotError. Please process it!");
                    break;
                }
            }
            else
            {
                std::cout << "WTF have you entered?!?\n";
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
        std::cout << winner.getName() << " won!\nPlease relaunch game if you want to play again";
    }
}

IPlayer& GameController::getCurrentPlayer(const Player player) const
{
    const int index = getIndexFromPlayer(player);
    return *m_players[index];
}
