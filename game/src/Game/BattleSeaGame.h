#pragma once
#include "Game/GameInterfaces.h"
#include "Core/CoreTypes.h"

class BattleSeaGame : public IBattleSeaGame
{
public:
	constexpr static uint8_t PLAYER_AMOUNT = 2;

	BattleSeaGame(const GameConfig& config);

	// Inherited via IBattleSeaGame
	virtual bool initShipPositionsForPlayer(const Player player, const std::vector<WarShip>& ships) override;
	virtual ShotError shootThePlayerGridAt(const CellIndex& cell) override;
	virtual bool startGame(const GameStartSettings& settings) override;
	virtual bool isGameOver() const override;
	virtual Player getCurrentPlayer() const override;
	virtual Player getInitialPlayer() const override;
	virtual Player getLocalPlayer() const override;
	virtual const GameGrid getPlayerGridInfo(const Player player) const override;
	virtual CellState getPlayerGridCellState(const Player player, const CellIndex& cell) const override;

	virtual const GameConfig& getAppliedConfig() const override;

private:
	void setGridCellState(GameGrid& outGrid, const CellIndex& cell, const CellState& state);
	void surroundDestroyedShip(GameGrid& outGrid, const WarShip& ship);

private:
	GameConfig m_config;

	std::array<GameGrid, PLAYER_AMOUNT> m_playerGrids;
	std::array<std::vector<WarShip>, PLAYER_AMOUNT> m_playerShips;

	Player m_currentPlayer;
	Player m_initialPlayer;
	Player m_localPlayer;

	bool m_hasGameFinished;
};

