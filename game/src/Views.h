#pragma once
#include <memory>
#include <string>

#include "Core/CoreTypes.h"     // for CellState, ShotError
#include "Game/GameGrid.h"      // GameGrid is typedef

class BattleSeaGame;
class CellIndex;

class IBattleSeaView {
public:
    virtual void renderGreetingToPlayer() = 0;
    virtual void renderGeneratedShips(const GameGrid& grid) = 0;
    virtual void renderGame() = 0;
    virtual void renderMessage(const std::string msg) = 0;
    virtual void renderRequestToTurn(const std::string playerName) = 0;
    virtual void renderShotError(const ShotError error) = 0;
    virtual void renderGameOver(const std::string winnerName, const bool isLocalPlayer) = 0;
};

class TerminalView : public IBattleSeaView 
{
public:
    TerminalView(const std::shared_ptr<BattleSeaGame>& game);

    virtual void renderGreetingToPlayer() override;
    virtual void renderGeneratedShips(const GameGrid& grid) override;
    virtual void renderGame() override;
    virtual void renderMessage(const std::string msg) override;
    virtual void renderRequestToTurn(const std::string playerName) override;
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
    const std::shared_ptr<BattleSeaGame> m_game;
};
