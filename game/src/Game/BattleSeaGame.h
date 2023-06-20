#pragma once
#include "Game/GameInterfaces.h"
#include "Core/CoreTypes.h"

class BattleSeaGame : public IBattleSeaGame
{
public:
	constexpr static uint8_t PLAYER_AMOUNT = 2;

	BattleSeaGame(std::unique_ptr<IWarShipGenerator>&& generator, const GameConfig& config);

	// Inherited via IBattleSeaGame
	virtual void generateShipsForPlayer(const Player player) override;
	virtual bool initShipPositionsForPlayer(const Player player, const std::vector<WarShip>& ships) override;
	virtual ShotError shootThePlayerGridAt(const CellIndex& cell) override;
	virtual void startGame(const Player initialPlayer) override;
	virtual bool isGameOver() const override;
	virtual Player getCurrentPlayer() const override;
	virtual Player getInitialPlayer() const override;
	virtual Player getLocalPlayer() const override;
	virtual void setLocalPlayer(const Player player) override;
	virtual const GridData getPlayerGridInfo(const Player player) const override;
	virtual CellState getPlayerGridCellState(const Player player, const CellIndex& cell) const override;

	virtual const GameConfig& getAppliedConfig() const override;

private:
	void setGridCellState(GridData& outGridData, const CellIndex& cell, const CellState& state);
	void surroundDestroyedShip(GridData& outGridData, const WarShip& ship);

private:
	std::unique_ptr<IWarShipGenerator> m_gridGenerator;
	GameConfig m_config;

	std::array<GridData, PLAYER_AMOUNT> m_playerGrids;
	std::array<std::vector<WarShip>, PLAYER_AMOUNT> m_playerShips;

	Player m_currentPlayer;
	Player m_initialPlayer;
	Player m_localPlayer;
};

