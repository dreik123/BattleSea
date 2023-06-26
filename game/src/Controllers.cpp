#include "Controllers.h"
#include "Views.h"

#include "Game/GameInterfaces.h"
#include "Game/Player/RealPlayer.h"
#include "Game/Player/SillyRandomBot.h"

#include <iostream>


GameController::GameController(std::shared_ptr<IBattleSeaGame>& game, std::shared_ptr<IBattleSeaView>& view)
    : m_game(game)
    , m_view(view)
    , m_shipsGenerator(new WarShipGenerator())
{
}

void GameController::runGame()
{
    m_players[0].reset(new RealPlayer(Player::Player1));
    // TODO AP please replace it with advanced AI bot when it's implemented
    m_players[1].reset(new SillyBotPlayer(Player::Player2, m_game));

    // [Temporary] TODO DS Player can regenerate ships many times before game start
    GameStartSettings settings;
    settings.initialPlayer = Player::Player1;
    settings.localPlayer = Player::Player1;
    settings.shipsForPlayer1 = m_shipsGenerator->generateShips(m_game->getAppliedConfig());
    settings.shipsForPlayer2 = m_shipsGenerator->generateShips(m_game->getAppliedConfig());

    if (!m_game->startGame(settings))
    {
        std::cout << "Invalid settings for game start!\n";
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
