#pragma once
#include "Core/CoreTypes.h"
#include "GameConfig.h"
#include "GameGrid.h"
#include "GameState.h"
#include "WarShip.h"

#include <array>
#include <vector>
#include <set>
#include <memory>


// Model is shortly presented by 2 game grids (10x10) for 2 players

// Every cell can represent one the states
//  - Concealed cell, which wasn't shot yet
//  - Missed shot, shot has been done, but no ship part is there
//              - Guarantted empty zone around destroyed ship, but for no is okay to present like MissedShot state
//  - Hit cell  (Damaged), shot has been done and some part of ship is damaged
//  - Destroyed, one or more cells which describes destroyed ship

// Do we have any difference in presentation of hit damaged and destroyed cells? - In concole - not sure, with GUI - yes, 100%

// Model must validate order of turns.

// Main functionality in the game in model context
// - initialization of two grids for players with specified ship positions
// - grid generation for player
// - own info about current player and validation turn order
// - shooting specified cell on the certain grid
// - game over check


struct GameStartSettings
{
	Player initialPlayer;
	Player localPlayer;

	std::vector<WarShip> shipsForPlayer1;
	std::vector<WarShip> shipsForPlayer2;
};

class EventBus;

class GameStateMachine
{
public:
	GameStateMachine(std::shared_ptr<EventBus>& bus);

	[[nodiscard]] inline const GameState getCurrentState() const { return m_currentState; }

	void switchToState(const GameState newState);

private:
	std::shared_ptr<EventBus> m_eventBus;
	GameState m_currentState;
};

class BattleSeaGame
{
public:
	constexpr static uint8_t PLAYER_AMOUNT = 2;

	BattleSeaGame(const GameConfig& config, std::shared_ptr<EventBus>& bus);
	~BattleSeaGame();

	void launch();

	bool initShipPositionsForPlayer(const Player player, const std::vector<WarShip>& ships);
	ShotError shootThePlayerGridAt(const CellIndex& cell);
	bool startBattle(const GameStartSettings& settings);
	bool isGameOver() const;
	Player getCurrentPlayer() const;
	Player getInitialPlayer() const;
	Player getLocalPlayer() const;
	const GameGrid getPlayerGridInfo(const Player player) const;
	CellState getPlayerGridCellState(const Player player, const CellIndex& cell) const;

	const GameConfig& getAppliedConfig() const;
	const GameState getCurrentState() const;

private:
	void setGridCellState(GameGrid& outGrid, const CellIndex& cell, const CellState& state);
	void surroundDestroyedShip(GameGrid& outGrid, const WarShip& ship, std::set<CellIndex>& outSurroundedCells);

private:
	GameConfig m_config;
	GameStateMachine m_stateMachine;

	std::array<GameGrid, PLAYER_AMOUNT> m_playerGrids;
	std::array<std::vector<WarShip>, PLAYER_AMOUNT> m_playerShips;

	Player m_currentPlayer;
	Player m_initialPlayer;
	Player m_localPlayer;

	bool m_hasGameFinished;

	std::shared_ptr<EventBus> m_eventBus;

	ListenerHandleId m_startScreenPassedEventHandleId;
	ListenerHandleId m_quitGameRequestEventHandleId;
};

