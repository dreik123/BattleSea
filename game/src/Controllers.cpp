#include "Controllers.h"
#include "Views.h"

#include "Game/BattleSeaGame.h"
#include "Game/GridUtilities.h"
#include "Game/Player/RealPlayer.h"
//#include "Game/Player/SillyRandomBot.h" // use for testing
#include "Game/Player/AIPlayer.h"

#include "Core/EventBus.h"
#include "Game/Events/Events.h"

#include <iostream>
#include <conio.h>
#include <chrono>


TerminalController::TerminalController(
    std::unique_ptr<BattleSeaGame>&& game,
    std::unique_ptr<IBattleSeaView>&& view,
    std::shared_ptr<EventBus>& bus)
    : m_game(std::move(game))
    , m_shipsGenerator(new WarShipGenerator())
    , m_eventBus(bus)
    , m_restartRequested(false)
{
    // TODO If game restart will be implemented, then probably need to keep renderer in controller to take ownership
    auto renderThreadFunc = [renderer = std::move(view)](std::stop_token token)
    {
        while (!token.stop_requested())
        {
            renderer->updateWithStopToken(token);
        }
    };
    m_renderThread = std::jthread(std::move(renderThreadFunc));
    m_renderThread.detach(); // let it lives separately
}

TerminalController::~TerminalController()
{
    m_renderThread.get_stop_source().request_stop();
}

void TerminalController::loopGame()
{
    system("cls"); // clean after possible restart

    m_game->launch();

    // TODO [optional] re-impl with event callbacks (BE AWARE: onBattleStarted contains while (game not over))
    while (m_game->getCurrentState() != GameState::QuitGame)
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

bool TerminalController::hasRestartRequested()
{
    return m_restartRequested;
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
        const GameGrid generatedGrid = GridUtilities::convertShipsToGrid(playerShips);

        const events::GridGeneratedEvent gridGeneratedEvent {.playerGridToConfirm = generatedGrid };
        m_eventBus->publish(gridGeneratedEvent);

        playerChoice = _getch();

        // simulate some waiting to let it be rendered with overpressing
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(50ms);
    } while (playerChoice != '\r' && playerChoice != '\n');

    return playerShips;
}

bool TerminalController::onStartScreen()
{
    int _ = _getch();

    const events::StartScreenPassedEvent startScreenPassedEvent;
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
    //m_players[0].reset(new AIPlayer(Player::Player1, m_game.get())); // can be useful
    m_players[1].reset(new AIPlayer(Player::Player2, m_game.get(), m_game->getAppliedConfig(), m_eventBus));
    //m_players[1].reset(new SillyBotPlayer(Player::Player2, m_game->getAppliedConfig(), m_eventBus)); // can be useful

    GameStartSettings settings;
    settings.initialPlayer = Player::Player1;
    settings.localPlayer = Player::Player1;
    settings.shipsForPlayer1 = playerShips;
    settings.shipsForPlayer2 = m_shipsGenerator->generateShips(m_game->getAppliedConfig());

    if (!m_game->startBattle(settings))    {
        std::cerr << "Invalid settings for game start!\n";
        return false;
    }

    return true;
}

bool TerminalController::onBattleStarted()
{
    // README Multiplayer requires understanding which Player value current player has
    // Important: Own grid must visualize ships as well, opponent's - no.

    while (!m_game->isGameOver())
    {
        IPlayer& currentPlayer = getCurrentPlayer(m_game->getCurrentPlayer());

        const events::PlayerTurnsEvent playerTurnsEvent
        {
            .playerName = currentPlayer.getName(),
            .isLocalPlayer = currentPlayer.isLocalPlayer(),
        };
        m_eventBus->publish(playerTurnsEvent);

        bool isValidTurn = false;
        do
        {
            const InputRequest userInput = currentPlayer.getInput();

            if (userInput.isQuitRequested)
            {
                const events::QuitGameRequestEvent quitGameRequestEvent;
                m_eventBus->publish(quitGameRequestEvent);

                return false;
            }

            if (userInput.isRestartRequested)
            {
                const events::GameRestartRequestEvent gameRestartRequestEvent;
                m_eventBus->publish(gameRestartRequestEvent);

                m_restartRequested = true;

                return false; // need to interrupt the execution
            }

            if (userInput.shotCell.has_value())
            {
                const ShotError result = m_game->shootThePlayerGridAt(userInput.shotCell.value());

                if (result != ShotError::Ok)
                {
                    const events::LocalShotErrorEvent localShotErrorEvent {.errorType = result};
                    m_eventBus->publish(localShotErrorEvent);
                }
                isValidTurn = result == ShotError::Ok;
            }
            else
            {
                std::cout << "WTF have you entered?!?\n";
            }
        } while (!isValidTurn);

        // simulate some waiting to let the grids to be rendered without overlapping with input
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(50ms);
    }
    return true;
}

bool TerminalController::onBattleFinished()
{
    // The current player hasn't been changed after last shot.
    IPlayer& winner = getCurrentPlayer(m_game->getCurrentPlayer());

    const events::GameOverEvent gameOverEvent {.winnerName = winner.getName(), .isLocalPlayer = winner.isLocalPlayer()};
    m_eventBus->publish(gameOverEvent);

    int playerChoice = _getch();

    m_restartRequested = playerChoice == 0x20; // space

    if (m_restartRequested)
    {
        const events::GameRestartRequestEvent gameRestartRequestEvent;
        m_eventBus->publish(gameRestartRequestEvent);
    }
    else
    {
        const events::QuitGameRequestEvent quitGameRequestEvent;
        m_eventBus->publish(quitGameRequestEvent);
    }

    return true;
}
