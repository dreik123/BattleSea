#include "BattleSeaGame.h"
#include "Game/WarShipGenerators.h"
#include "Game/WarShip.h"

#include "Core/EventBus.h"
#include "Game/Events/Events.h"

#include "Game/GridUtilities.h"


GameStateMachine::GameStateMachine(std::shared_ptr<EventBus>& bus)
    : m_eventBus(bus)
    , m_currentState(GameState::Unitialized)
{
}

void GameStateMachine::switchToState(const GameState newState)
{
    GameState oldState = m_currentState;
    m_currentState = newState;

    const events::GameStateChangedEvent gameStateChangedEvent {.oldState = oldState, .newState = newState };
    m_eventBus->publish(gameStateChangedEvent);
}



BattleSeaGame::BattleSeaGame(const GameConfig& config, std::shared_ptr<EventBus>& bus)
    : m_config(config)
    , m_stateMachine(bus)
    , m_playerGrids{}
    , m_playerShips{}
    , m_currentPlayer(Player::Invalid)
    , m_initialPlayer(Player::Invalid)
    , m_localPlayer(Player::Invalid)
    , m_hasGameFinished(false)
    , m_eventBus(bus)
{
    m_startScreenPassedEventHandleId = m_eventBus->subscribe<events::StartScreenPassedEvent>([this](const std::any& _)
        {
            m_stateMachine.switchToState(GameState::ShipsSetup);
        });
    m_quitGameRequestEventHandleId = m_eventBus->subscribe<events::QuitGameRequestEvent>([this](const std::any& _)
        {
            m_stateMachine.switchToState(GameState::QuitGame);
        });
}

BattleSeaGame::~BattleSeaGame()
{
    m_eventBus->unsubscribe(m_startScreenPassedEventHandleId);
    m_eventBus->unsubscribe(m_quitGameRequestEventHandleId);
}

void BattleSeaGame::launch()
{
    m_stateMachine.switchToState(GameState::StartScreen);
}

bool BattleSeaGame::initShipPositionsForPlayer(const Player player, const std::vector<WarShip>& ships)
{
    // TODO [MP MUST HAVE] validate ships position at least under some macro definition (debug for ex.)
    const bool isValidated = true;
    if (!isValidated)
    {
        return false;
    }

    m_playerShips[getIndexFromPlayer(player)] = ships;

    return true;
}

ShotError BattleSeaGame::shootThePlayerGridAt(const CellIndex& cell)
{
    assert(getCurrentPlayer() != Player::Invalid);

    const Player oppositePlayer = getOppositePlayer(getCurrentPlayer());
    const int oppositePlayerIndex = getIndexFromPlayer(oppositePlayer);
    GameGrid& oppositeGrid = m_playerGrids[oppositePlayerIndex];
    std::vector<WarShip>& ships = m_playerShips[oppositePlayerIndex];

    if (cell.x() < 0 || cell.x() >= m_config.rowsCount || cell.y() < 0 || cell.y() >= m_config.columnsCount)
    {
        // Invalid input from player (out of current grid size)
        return ShotError::OutOfGrid;
    }

    if (oppositeGrid.at(cell) != CellState::Concealed)
    {
        return ShotError::RepeatedShot;
    }

    bool successfulShot = false;

    for (WarShip& ship : ships)
    {
        if (ship.isDestroyed() || !ship.doesOccupyTheCell(cell))
        {
            continue;
        }

        ship.shootShipAtCell(cell);

        if (!ship.isDestroyed())
        {
            setGridCellState(oppositeGrid, cell, CellState::Damaged);

            const events::ShipDamagedEvent shipDamagedEvent {.injuredPlayer = oppositePlayer, .shot = cell};
            m_eventBus->publish(shipDamagedEvent);
        }
        else
        {
            // Mark the whole ship in destroyed
            for (const CellIndex& shipCell : ship.getOccupiedCells())
            {
                setGridCellState(oppositeGrid, cell, CellState::Destroyed);
            }

            // Wrap all surrounded cells of the ship in missed
            std::set<CellIndex> surroundedCells;
            surroundDestroyedShip(oppositeGrid, ship, surroundedCells);

            const events::ShipDestroyedEvent shipDestroyedEvent {.injuredPlayer = oppositePlayer, .ship = ship, .surroundedCells = surroundedCells};
            m_eventBus->publish(shipDestroyedEvent);

            m_hasGameFinished = std::all_of(ships.cbegin(), ships.cend(), [](const WarShip& ship)
                {
                    return ship.isDestroyed();
                });
        }
        successfulShot = true;
        break;
    }

    if (!successfulShot)
    {
        setGridCellState(oppositeGrid, cell, CellState::Missed);
        const events::ShotMissedEvent shotMissedEvent{.shootingPlayer = m_currentPlayer, .shot = cell};
        m_eventBus->publish(shotMissedEvent);
    }

    // HACK there is no functionality for terminal renderer to present certain shot, instead it refreshes entire grid
    const events::FullGridsSyncEvent fullGridsSyncEvent
    {
        .firstGrid = getPlayerGridInfo(Player::Player1),
        .secondGrid = getPlayerGridInfo(Player::Player2),
    };
    m_eventBus->publish(fullGridsSyncEvent);

    if (m_hasGameFinished)
    {
        m_stateMachine.switchToState(GameState::GameOver);

        const events::GameFinishedEvent gameFinishedEvent{.winner = m_currentPlayer, .loser = oppositePlayer};
        m_eventBus->publish(gameFinishedEvent);
    }
    else if (!successfulShot)
    {
        const Player prevPlayer = m_currentPlayer;
        m_currentPlayer = getOppositePlayer(m_currentPlayer);
        const events::PlayerSwitchedEvent playerSwitchedEvent{.previousPlayer = prevPlayer, .nextPlayer = m_currentPlayer};
        m_eventBus->publish(playerSwitchedEvent);
    }

    return ShotError::Ok;
}

bool BattleSeaGame::startBattle(const GameStartSettings& settings)
{
    m_initialPlayer = settings.initialPlayer;
    m_currentPlayer = settings.initialPlayer;
    m_localPlayer = settings.localPlayer;

    if (!initShipPositionsForPlayer(Player::Player1, settings.shipsForPlayer1))
    {
        return false;
    }
    if (!initShipPositionsForPlayer(Player::Player2, settings.shipsForPlayer2))
    {
        return false;
    }

    m_stateMachine.switchToState(GameState::Battle);

    const events::FullGridsSyncEvent fullGridsSyncEvent
    {
        .firstGrid = getPlayerGridInfo(Player::Player1),
        .secondGrid = getPlayerGridInfo(Player::Player2),
    };
    m_eventBus->publish(fullGridsSyncEvent);

    return true;
}

bool BattleSeaGame::isGameOver() const
{
    return m_hasGameFinished;
}

Player BattleSeaGame::getCurrentPlayer() const
{
    return m_currentPlayer;
}

Player BattleSeaGame::getInitialPlayer() const
{
    return m_initialPlayer;
}

Player BattleSeaGame::getLocalPlayer() const
{
    return m_localPlayer;
}

const GameGrid BattleSeaGame::getPlayerGridInfo(const Player player) const
{
    // Create a copy of grid data to make the ship data visible for local player only
    const int playerIndex = getIndexFromPlayer(player);

    if (getLocalPlayer() != player)
    {
        return m_playerGrids[playerIndex];
    }

    GameGrid gridCopy = m_playerGrids[playerIndex];
    const std::vector<WarShip>& warShips = m_playerShips[playerIndex];

    for (const WarShip& ship : warShips)
    {
        for (const CellIndex& cell : ship.getOccupiedCells())
        {
            if (gridCopy.at(cell) == CellState::Concealed)
            {
                gridCopy.at(cell) = CellState::Ship;
            }
        }
    }

    return gridCopy;
}

CellState BattleSeaGame::getPlayerGridCellState(const Player player, const CellIndex& cell) const
{
    return getPlayerGridInfo(player).at(cell);
}

const GameConfig& BattleSeaGame::getAppliedConfig() const
{
    return m_config;
}

const GameState BattleSeaGame::getCurrentState() const
{
    return m_stateMachine.getCurrentState();
}

void BattleSeaGame::setGridCellState(GameGrid& outGrid, const CellIndex& cell, const CellState& state)
{
    outGrid.at(cell) = state;
}

void BattleSeaGame::surroundDestroyedShip(GameGrid& outGrid, const WarShip& ship, std::set<CellIndex>& outSurroundedCells)
{
    // Forehead solution
    for (const CellIndex& cell : ship.getOccupiedCells())
    {
        GridUtilities::safeCellWalkthrough(cell, [&outGrid, &outSurroundedCells](int newX, int newY)
        {
            if (outGrid[newX][newY] == CellState::Concealed)
            {
                outGrid[newX][newY] = CellState::Missed;
            }

            // Missed is checked here to cover all cells arounds even missed ones
            if (outGrid[newX][newY] == CellState::Concealed || outGrid[newX][newY] == CellState::Missed)
            {
                outSurroundedCells.insert(CellIndex{ newX, newY });
            }
        });
    }
}
