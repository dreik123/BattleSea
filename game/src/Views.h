#pragma once
#include "Core/CoreTypes.h"          // for GridData

#include <memory>

struct IBattleSeaGame;
class CellIndex;

struct IBattleSeaView {
    virtual void RenderGame() = 0;
};

class TerminalView : public IBattleSeaView 
{
public:
    TerminalView(const std::shared_ptr<IBattleSeaGame>& InGame);

    virtual void RenderGame() override;

private:
    // DS TODO implement the method for both grids
    void RenderGrid(const GridData& InGridData);
    void RenderCell(const CellIndex& InIndex, const CellState InState);

    void RenderHorizontalDelimitersPerCell();
    void RenderVerticalDelimitersPerCell();

    void RenderLetterAxisWithAlignment(const char InSymbol);

    void RenderSymbolNTimes(const char InSymbol, const unsigned int InTimes);

protected:
    const std::shared_ptr<IBattleSeaGame> Game;
};
