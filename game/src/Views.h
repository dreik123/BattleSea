#pragma once
#include "CoreTypes.h"

struct IBattleSeaGame;
class CellIndex;

struct IBattleSeaView {
    virtual void RenderGame() = 0;
};

class TerminalView : public IBattleSeaView 
{
public:
    TerminalView(const IBattleSeaGame* InGame)
        : Game(InGame)
    {
    }

    virtual void RenderGame() override;

private:
    // DS TODO implement the method for both grids
    void RenderGrid(const GridData& InGridData);
    void RenderCell(const CellIndex& InIndex, const CellState InState);

protected:
    const IBattleSeaGame* Game;
};
