#pragma once
#include "Game/GameInterfaces.h"
#include "Core/CoreTypes.h"

class BattleSeaGame : public IBattleSeaGame
{
public:
	constexpr static uint8_t kPlayerAmount = 2;

	BattleSeaGame(std::unique_ptr<IWarShipGenerator>&& InGenerator, const GameConfig& InConfig);

	// Inherited via IBattleSeaGame
	virtual void GenerateShipsForPlayer(const EPlayer player) override;
	virtual bool InitShipPositionsForPlayer(const EPlayer player, const std::vector<WarShip>& ships) override;
	virtual bool ShootThePlayerGridAt(const CellIndex& cell) override;
	virtual void StartGame(const EPlayer initialPlayer) override;
	virtual bool IsGameOver() const override;
	virtual EPlayer GetCurrentPlayer() const override;
	virtual EPlayer GetInitialPlayer() const override;
	virtual EPlayer GetLocalPlayer() const override;
	virtual void SetLocalPlayer(const EPlayer player) override;
	virtual const GridData GetPlayerGridInfo(const EPlayer player) const override;
	virtual CellState GetPlayerGridCellState(const EPlayer player, const CellIndex& cell) const override;

	static int GetIndexFromPlayer(const EPlayer& player);

private:
	void SetGridCellState(GridData& outGridData, const CellIndex& cell, const CellState& state);
	void SurroundDestroyedShip(GridData& outGridData, const WarShip& ship);

private:
	std::unique_ptr<IWarShipGenerator> GridGenerator;
	GameConfig Config;

	std::array<GridData, kPlayerAmount> PlayerGrids;
	std::array<std::vector<WarShip>, kPlayerAmount> PlayerShips;

	// TODO use bit fields for EPlayer
	EPlayer CurrentPlayer;
	EPlayer InitialPlayer;
	EPlayer LocalPlayer;
};

