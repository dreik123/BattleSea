#pragma once
#include "Core/CoreTypes.h"          // for GridData

#include <memory>

class IBattleSeaGame;
class CellIndex;

class IBattleSeaView {
public:
    virtual void renderGame() = 0;
};

class TerminalView : public IBattleSeaView 
{
public:
    TerminalView(const std::shared_ptr<IBattleSeaGame>& game);

    virtual void renderGame() override;

private:
    void renderSingleGrid(const GridData& gridData);
    void renderTwoGrids(const GridData& gridDataLeft, const GridData& gridDataRight, const bool isHorizontally = true);
    void renderCell(const CellIndex& index, const CellState state);

    void renderHorizontalDelimitersPerCell();
    void renderVerticalDelimitersPerCell();
    void renderGridOffset();

    void renderLetterAxisWithAlignment(const char symbol);

    void renderSymbolNTimes(const char symbol, const unsigned int times);

protected:
    const std::shared_ptr<IBattleSeaGame> m_game;
};
