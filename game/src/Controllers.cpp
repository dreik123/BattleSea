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


TerminalController::TerminalController(
    std::unique_ptr<BattleSeaGame>&& game,
    std::unique_ptr<IBattleSeaView>&& view,
    std::shared_ptr<EventBus>& bus)
    : m_game(std::move(game))
    , m_renderer(std::move(view))
    , m_shipsGenerator(new WarShipGenerator())
    , m_eventBus(bus)
{
}

void TerminalController::loopGame()
{
    m_game->launch();

    while (true)
    {
        switch (m_game->getCurrentState())
        {
        case GameState::Unitialized:
        {
            assert(false && "Invalid game state!!!");
            break;
        }
        case GameState::StartScreen:
        {
            onStartScreen();
            break;
        }
        case GameState::ShipsSetup:
        {
            if (!onShipsSetup())
            {
                return;
            }
            break;
        }
        case GameState::Battle:
        {
            if (!onBattleStarted())
            {
                return;
            }
            break;
        }
        case GameState::GameOver:
        {
            assert(m_game->isGameOver());

            onBattleFinished();
            return;
        }
        default:
            assert(false && "Unprocessed game state in TerminalController::loopGame()");
            break;
        }
    }
}

IPlayer& TerminalController::getCurrentPlayer(const Player player) const
{
    const int index = getIndexFromPlayer(player);
    return *m_players[index];
}

const std::vector<WarShip> TerminalController::getShipsFromPlayer(const Player _)
{
    // Regenerate ships for player while space is pressed (until enter)
    std::vector<WarShip> playerShips;
    char playerChoice = 0;
    do
    {
        playerShips = m_shipsGenerator->generateShips(m_game->getAppliedConfig());
        const GameGrid gridToPresent = GridUtilities::convertShipsToGrid(playerShips);
        m_renderer->renderGeneratedShips(gridToPresent);
        // TODO publish(eventWithShips);
        std::cout << "Do you like this setup?\nEnter - approve! Any button - regenerate\n";

        playerChoice = _getch();
    } while (playerChoice != '\r' && playerChoice != '\n');

    return playerShips;
}

bool TerminalController::onStartScreen()
{
    // should renderer be on separate thread? how to sync threads then?
    m_renderer->renderStartScreen();

    // input from user should be abstraction
    int _ = _getch();

    events::StartScreenPassedEvent startScreenPassedEvent;
    m_eventBus->publish(startScreenPassedEvent);

    return true;
}

bool TerminalController::onShipsSetup()
{
    // Encapsulated logic of getting ships from controller
    const std::vector<WarShip> playerShips = getShipsFromPlayer(Player::Player1);

    system("cls");

    // Game init
    m_players[0].reset(new RealPlayer(Player::Player1));
    //m_players[0].reset(new SillyBotPlayer(Player::Player1, m_game.get())); // can be useful
   // TODO AP please replace it with advanced AI bot when it's implemented
    m_players[1].reset(new SillyBotPlayer(Player::Player2, m_game.get()));

    GameStartSettings settings;
    settings.initialPlayer = Player::Player1;
    settings.localPlayer = Player::Player1;
    settings.shipsForPlayer1 = playerShips;
    settings.shipsForPlayer2 = m_shipsGenerator->generateShips(m_game->getAppliedConfig());

    if (!m_game->startBattle(settings))
    {
        std::cerr << "Invalid settings for game start!\n";
        return false;
    }

    return true;
}

bool TerminalController::onBattleStarted()
{
    // README Multiplayer requires understanding which Player value current player has
    // Important: Own grid must visualize ships as well, opponent's - no.

    // Shows grids before first turn
    m_renderer->renderGameGrids(
        m_game->getPlayerGridInfo(Player::Player1),
        m_game->getPlayerGridInfo(Player::Player2)
    );

    //bool hasGameBeenInterrupted = false;
    while (/*!hasGameBeenInterrupted && */!m_game->isGameOver())
    {
        IPlayer& currentPlayer = getCurrentPlayer(m_game->getCurrentPlayer());
        std::cout << currentPlayer.getName() << " turns:\n";

        bool isValidTurn = false;
        do
        {
            const InputRequest userInput = currentPlayer.getInput();

            if (userInput.isQuitRequested)
            {
                //hasGameBeenInterrupted = true;
                return false;
            }

            if (userInput.shotCell.has_value())
            {
                const ShotError result = m_game->shootThePlayerGridAt(userInput.shotCell.value());

                m_renderer->renderShotError(result); // battle state required
                isValidTurn = result == ShotError::Ok;
            }
            else
            {
                std::cout << "WTF have you entered?!?\n";
            }
        } while (!isValidTurn);

        // Clear screen to refresh the grids in place
        system("cls");

        m_renderer->renderGameGrids(
            m_game->getPlayerGridInfo(Player::Player1),
            m_game->getPlayerGridInfo(Player::Player2)
        );
    }
    return true;
}

bool TerminalController::onBattleFinished()
{
    // The current player hasn't been changed after last shot.
    IPlayer& winner = getCurrentPlayer(m_game->getCurrentPlayer());
    m_renderer->renderGameOver(winner.getName(), winner.isLocalPlayer());
    std::cout << "Please relaunch game if you want to play again\n";

    return true;
}
