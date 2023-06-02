#pragma once
#include "Game/GameInterfaces.h"
#include "Core/CoreTypes.h"


class BattleSeaGame : public IBattleSeaGame
{
public:
	constexpr static uint8_t kPlayerAmount = 2;

	BattleSeaGame();

	// Inherited via IBattleSeaGame
	//virtual void SetWarShipGenerator(const std::unique_ptr<IWarShipGenerator>& generator) override;
	virtual void GenerateShipsForPlayer(const EPlayer player, const GameConfig& params) override;
	virtual bool InitShipPositionsForPlayer(const EPlayer player, const std::vector<WarShip>& ships) override;
	virtual bool ShootThePlayerGridAt(const EPlayer player, const CellIndex& cell) override;
	virtual bool IsGameOver() const override;
	virtual EPlayer GetCurrentPlayer() const override;
	virtual void SetInitialPlayer(EPlayer player) override;
	virtual const GridData& GetPlayerGridInfo(const EPlayer player) const override;
	virtual CellState GetPlayerGridCellState(const EPlayer player, const CellIndex& cell) const override;

	static int GetIndexFromPlayer(const EPlayer& player);

private:
	void SetGridCellState(GridData& outGridData, const CellIndex& cell, const CellState& state);
	void SurroundDestroyedShip(GridData& outGridData, const WarShip& ship);

	EPlayer GetNextPlayer() const;

private:
	std::unique_ptr<IWarShipGenerator> GridGenerator;
	std::array<GridData, kPlayerAmount> PlayerGrids;
	std::array<std::vector<WarShip>, kPlayerAmount> PlayerShips;

	EPlayer CurrentPlayer;
};

