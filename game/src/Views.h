#pragma once
#include <memory>

#include "Core/CoreTypes.h"     // for CellState
#include "Game/GameGrid.h"      // GameGrid is typedef

class IBattleSeaGame;
class CellIndex;

class IBattleSeaView {
public:
    virtual void renderGame() = 0;
    virtual void renderGeneratedShips(const GameGrid& grid) = 0;
};

class TerminalView : public IBattleSeaView 
{
public:
    TerminalView(const std::shared_ptr<IBattleSeaGame>& game);

    virtual void renderGame() override;
    virtual void renderGeneratedShips(const GameGrid& grid) override;

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
    const std::shared_ptr<IBattleSeaGame> m_game;
};
