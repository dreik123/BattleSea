#pragma once
#include "Game/GameInterfaces.h"
#include "Core/CoreTypes.h"

class BattleSeaGame : public IBattleSeaGame
{
public:
	constexpr static uint8_t S_PLAYER_AMOUNT = 2;

	BattleSeaGame(std::unique_ptr<IWarShipGenerator>&& generator, const GameConfig& config);

	// Inherited via IBattleSeaGame
	virtual void generateShipsForPlayer(const EPlayer player) override;
	virtual bool initShipPositionsForPlayer(const EPlayer player, const std::vector<WarShip>& ships) override;
	virtual bool shootThePlayerGridAt(const CellIndex& cell) override;
	virtual void startGame(const EPlayer initialPlayer) override;
	virtual bool isGameOver() const override;
	virtual EPlayer getCurrentPlayer() const override;
	virtual EPlayer getInitialPlayer() const override;
	virtual EPlayer getLocalPlayer() const override;
	virtual void setLocalPlayer(const EPlayer player) override;
	virtual const GridData getPlayerGridInfo(const EPlayer player) const override;
	virtual CellState getPlayerGridCellState(const EPlayer player, const CellIndex& cell) const override;

	static int getIndexFromPlayer(const EPlayer& player);

private:
	void setGridCellState(GridData& outGridData, const CellIndex& cell, const CellState& state);
	void surroundDestroyedShip(GridData& outGridData, const WarShip& ship);

private:
	std::unique_ptr<IWarShipGenerator> m_gridGenerator;
	GameConfig m_config;

	std::array<GridData, S_PLAYER_AMOUNT> m_playerGrids;
	std::array<std::vector<WarShip>, S_PLAYER_AMOUNT> m_playerShips;

	static_assert(static_cast<int>(EPlayer::Count) < 4);
	// 2 bits covers 0..3 possible values, so it's okay to keep EPlayer values in 2 bits
	EPlayer m_currentPlayer : 2;
	EPlayer m_initialPlayer : 2;
	EPlayer m_localPlayer : 2;
	char : 0; // alignment to fit current byte
};

