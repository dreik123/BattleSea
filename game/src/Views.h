#pragma once
#include <memory>
#include <string>

#include "Core/CoreTypes.h"     // for CellState, ShotError
#include "Core/EventBus.h"
#include "Game/GameGrid.h"      // CONSIDER FWD DECLARATION
#include "Game/GameState.h"

class CellIndex;
class EventBus;

class IBattleSeaView
{
public:
    virtual void update() = 0;

    virtual void renderStartScreen() = 0;
    virtual void renderGeneratedShips(const GameGrid& grid) = 0;
    virtual void renderGameGrids(const GameGrid modelData1, const GameGrid modelData2) = 0;
    virtual void renderShotError(const ShotError error) = 0;
    virtual void renderGameOver(const std::string winnerName, const bool isLocalPlayer) = 0;
};


class TerminalView : public IBattleSeaView
{
public:
    TerminalView(std::shared_ptr<EventBus>& bus);

    virtual void update() { /* TODO impl async processing */ }

    virtual void renderStartScreen() override;
    virtual void renderGeneratedShips(const GameGrid& grid) override;
    virtual void renderGameGrids(const GameGrid modelData1, const GameGrid modelData2) override;
    virtual void renderShotError(const ShotError error) override;
    virtual void renderGameOver(const std::string winnerName, const bool isLocalPlayer) override;

private:
    void renderSingleGrid(const GameGrid& grid);
    void renderTwoGrids(const GameGrid& gridLeft, const GameGrid& gridRight, const bool isHorizontally = true);
    void renderCell(const CellIndex& index, const CellState state);

    void renderHorizontalDelimitersPerCell();
    void renderVerticalDelimitersPerCell();
    void renderGridOffset();

    void renderLetterAxisWithAlignment(const char symbol);

    void renderSymbolNTimes(const char symbol, const unsigned int times);

private:
    void onGameStateUpdated(const GameState& state);
    void subscribeToEvents();

protected:
    std::shared_ptr<EventBus> m_eventBus;
};
