#pragma once
#include <memory>
#include <string>
#include <stop_token>

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
    virtual void updateWithStopToken(std::stop_token token) = 0;


    virtual void renderStartScreen() = 0;
    virtual void renderGeneratedShips(const GameGrid& grid) = 0;
    virtual void renderRequestToTurn(const std::string playerName, const bool isLocalPlayer) = 0;
    virtual void renderGameGrids(const GameGrid modelData1, const GameGrid modelData2) = 0;
    virtual void renderShotError(const ShotError error) = 0;
    virtual void renderGameOver(const std::string winnerName, const bool isLocalPlayer) = 0;
};


class TerminalView : public IBattleSeaView
{
public:
    TerminalView(std::shared_ptr<EventBus>& bus);

    virtual void update() override;
    virtual void updateWithStopToken(std::stop_token token) override;

    // TODO friend executor and private methods or keep public
    virtual void renderStartScreen() override;
    virtual void renderGeneratedShips(const GameGrid& grid) override;
    virtual void renderRequestToTurn(const std::string playerName, const bool isLocalPlayer) override;
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

protected:
    std::shared_ptr<EventBus> m_eventBus;
    std::unique_ptr<class RenderInstructionExecutor> m_renderExecutorImpl;
};
