#include "Controllers.h"
#include "Views.h"

#include "Game/GameInterfaces.h"
#include "Game/Player/RealPlayer.h"
#include "Game/Player/SillyRandomBot.h"

#include <iostream>


GameController::GameController(std::shared_ptr<IBattleSeaGame>& game, std::shared_ptr<IBattleSeaView>& view)
    : m_game(game)
    , m_view(view)
    , m_gridGenerator(new WarShipGenerator())
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
    settings.firstShips = m_gridGenerator->generateShips(m_game->getAppliedConfig());
    settings.secondShips = m_gridGenerator->generateShips(m_game->getAppliedConfig());
    m_game->startGame(settings);

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
