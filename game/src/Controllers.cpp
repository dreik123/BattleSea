#include "Controllers.h"
#include "Views.h"

#include "Game/GameInterfaces.h"
#include "Game/Player/RealPlayer.h"
#include "Game/Player/SillyRandomBot.h"

#include <iostream>


GameController::GameController(std::shared_ptr<IBattleSeaGame>& game, std::shared_ptr<IBattleSeaView>& view)
    : m_game(game)
    , m_view(view)
{
}

void GameController::runGame()
{
    m_players[0].reset(new RealPlayer(Player::Player1));
    // TODO AP please replace it with advanced AI bot when it's implemented
    m_players[1].reset(new SillyBotPlayer(Player::Player2, m_game));

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
        IPlayer& currentPlayer = getCurrentPlayer(m_game->getCurrentPlayer());


        // Shows grids before first turn
        m_view->renderGame();

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
    }
}

IPlayer& GameController::getCurrentPlayer(const Player player) const
{
    const int index = getIndexFromPlayer(player);
    return *m_players[index];
}
