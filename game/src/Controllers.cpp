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
#include <chrono>


// TODO input from user should be abstraction, but probably controller should process it instead of player


TerminalController::TerminalController(
    std::unique_ptr<BattleSeaGame>&& game,
    std::unique_ptr<IBattleSeaView>&& view,
    std::shared_ptr<EventBus>& bus)
    : m_game(std::move(game))
    , m_renderer(std::move(view))
    , m_shipsGenerator(new WarShipGenerator())
    , m_eventBus(bus)
{
    auto renderThreadFunc = [renderer = std::move(m_renderer)](std::stop_token token)
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
    m_game->launch();

    // TODO re-impl with event callbacks
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

                return true;
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

    int _ = _getch();
    // [OPTIONAL] TODO consider option to restart game without closing it

    const events::QuitGameRequestEvent quitGameRequestEvent;
    m_eventBus->publish(quitGameRequestEvent);

    return true;
}
