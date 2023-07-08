#include "Controllers.h"
#include "Views.h"

#include "Game/BattleSeaGame.h"
#include "Game/GridUtilities.h"
#include "Game/Player/RealPlayer.h"
#include "Game/Player/SillyRandomBot.h"

#include "Core/EventBus.h"
#include "Game/Events/Events.h"

#include <iostream>
#include <conio.h>


GameController::GameController(
    std::weak_ptr<BattleSeaGame>& game,
    std::shared_ptr<IBattleSeaView>& view,
    std::shared_ptr<EventBus>& bus)
    : m_game(game)
    , m_renderer(view)
    , m_shipsGenerator(new WarShipGenerator())
    , m_eventBus(bus)
{
}

void GameController::runGame()
{
    if (m_game.expired())
    {
        std::cerr << "Game instance is invalid\n";
        return;
    }

    auto gameInstance = m_game.lock();

    m_renderer->renderStartScreen();
    int _ = _getch();

    events::StartScreenPassedEvent startScreenPassedEvent;
    m_eventBus->publish(startScreenPassedEvent);

    // README For now ships generation is performed here in controller,
    // but potentially it might be carried out to IPlayer interface.

    // Regenerate ships for player while space is pressed (until enter)
    std::vector<WarShip> playerShips;
    char playerChoice = 0;
    do
    {
        playerShips = m_shipsGenerator->generateShips(gameInstance->getAppliedConfig());
        const GameGrid gridToPresent = GridUtilities::convertShipsToGrid(playerShips);
        m_renderer->renderGeneratedShips(gridToPresent);
        m_renderer->renderMessage("Do you like this setup?\nEnter - approve! Any button - regenerate\n");

        playerChoice = _getch();
    } while (playerChoice != '\r' && playerChoice != '\n');

    system("cls");

    // Game init
    m_players[0].reset(new RealPlayer(Player::Player1));
    // m_players[0].reset(new SillyBotPlayer(Player::Player1, m_game)); // can be useful
    // TODO AP please replace it with advanced AI bot when it's implemented
    m_players[1].reset(new SillyBotPlayer(Player::Player2, m_game));

    GameStartSettings settings;
    settings.initialPlayer = Player::Player1;
    settings.localPlayer = Player::Player1;
    settings.shipsForPlayer1 = playerShips;
    settings.shipsForPlayer2 = m_shipsGenerator->generateShips(gameInstance->getAppliedConfig());

    if (!gameInstance->startBattle(settings))
    {
        std::cerr << "Invalid settings for game start!\n";
        return;
    }

    // Shows grids before first turn
    m_renderer->renderGame();

    bool hasGameBeenInterrupted = false;
    while (!hasGameBeenInterrupted && !gameInstance->isGameOver())
    {
        IPlayer& currentPlayer = getCurrentPlayer(gameInstance->getCurrentPlayer());
        std::cout << currentPlayer.getName() << " turns:\n";

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
                const ShotError result = gameInstance->shootThePlayerGridAt(userInput.shotCell.value());

                m_renderer->renderShotError(result);
                isValidTurn = result == ShotError::Ok;
            }
            else
            {
                m_renderer->renderMessage("WTF have you entered?!?\n");
            }
        } while (!isValidTurn);

        // Clear screen to refresh the grids in place
        system("cls");

        m_renderer->renderGame();
    }

    if (gameInstance->isGameOver())
    {
        // The current player hasn't been changed after last shot.
        IPlayer& winner = getCurrentPlayer(gameInstance->getCurrentPlayer());
        m_renderer->renderGameOver(winner.getName(), winner.isLocalPlayer());
        m_renderer->renderMessage("Please relaunch game if you want to play again\n");
    }
}

IPlayer& GameController::getCurrentPlayer(const Player player) const
{
    const int index = getIndexFromPlayer(player);
    return *m_players[index];
}
