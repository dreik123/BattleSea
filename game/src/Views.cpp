#include "Views.h"
#include "Game/GameInterfaces.h"         // for IBattleSeaGame

#include <iostream>  // is temporary used for console presentation


// TODO DS adjust with code-style as soon as we choose it
// Constants
namespace
{
    constexpr unsigned char LEFT_TOP_EDGE = 201;
    constexpr unsigned char RIGHT_TOP_EDGE = 187;
    constexpr unsigned char LEFT_BOTTOM_EDGE = 200;
    constexpr unsigned char RIGHT_BOTTOM_EDGE = 188;

    constexpr unsigned char LEFT_CENTER_PIECE = 204;
    constexpr unsigned char RIGHT_CENTER_PIECE = 185;
    constexpr unsigned char TOP_CENTER_PIECE = 203;
    constexpr unsigned char BOTTOM_CENTER_PIECE = 202;

    constexpr unsigned char VERTICAL_PIECE = 186;
    constexpr unsigned char HORIZONTAL_PIECE = 205;

    constexpr unsigned char CENTER_PIECE = 206;

    constexpr unsigned char EMPTY_FILLER = 176;
}


constexpr static uint8_t HorizontalSymbolsAmountPerCell = 3;
constexpr static uint8_t VerticalSymbolsAmountPerCell = 1;

// TODO probably make sense to take the data from model when model is implemented
constexpr static std::array<char, 10> RowAxisNames =
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'
};


static const GridData GetTestData()
{
    // Temp data for test
    GridData gridData;
    for (auto& row : gridData)
    {
        for (auto& cell : row)
        {
            cell = CellState::Concealed;
        }
    }

    gridData[6][6] = CellState::Missed;
    gridData[0][0] = CellState::Missed;
    gridData[1][1] = CellState::Damaged;

    gridData[2][3] = CellState::Destroyed;

    return gridData;
}

TerminalView::TerminalView(const std::shared_ptr<IBattleSeaGame>& InGame)
    : Game(InGame)
{
}

void TerminalView::RenderGame()
{
    // TODO DS link to model data
    const GridData modelData = GetTestData();
    RenderGrid(modelData);
}

void TerminalView::RenderGrid(const GridData& InGridData)
{
    assert(InGridData.size() != 0 && InGridData.front().size() != 0);

    const size_t RowCount = InGridData.size();
    const size_t ColCount = InGridData[0].size();

    RenderLetterAxisWithAlignment(' ');
    for (int i(1); i <= ColCount; i++)
    {
        // TODO improve this to be dependent on some formatting approach
        std::cout << "  " << i << " ";
    }
    std::cout << std::endl;

    RenderLetterAxisWithAlignment(' ');
    std::cout << LEFT_TOP_EDGE;
    for (int i(0); i < RowCount - 1; i++)
    {
        RenderHorizontalDelimitersPerCell();
        std::cout << TOP_CENTER_PIECE;
    }

    RenderHorizontalDelimitersPerCell();
    std::cout << RIGHT_TOP_EDGE << std::endl;

    for (int i(0); i < ColCount - 1; i++)
    {
        RenderLetterAxisWithAlignment(RowAxisNames[i]);
        for (int j(0); j < RowCount; j++)
        {
            RenderVerticalDelimitersPerCell();
            const CellIndex Index = CellIndex(i, j);
            RenderCell(Index, InGridData[i][j]);
        }
        RenderVerticalDelimitersPerCell();
        std::cout << std::endl;

        RenderLetterAxisWithAlignment(' ');
        std::cout << LEFT_CENTER_PIECE;
        for (int j(0); j < RowCount - 1; j++)
        {
            RenderHorizontalDelimitersPerCell();
            std::cout << CENTER_PIECE;
        }
        RenderHorizontalDelimitersPerCell();
        std::cout << RIGHT_CENTER_PIECE << std::endl;
    }

    RenderLetterAxisWithAlignment(RowAxisNames.back());
    for (int i(0); i < RowCount; i++)
    {
        RenderVerticalDelimitersPerCell();
        const CellIndex Index = CellIndex(RowCount - 1, i);
        RenderCell(Index, InGridData.back()[i]);
    }

    RenderVerticalDelimitersPerCell();
    std::cout << std::endl;

    RenderLetterAxisWithAlignment(' ');
    std::cout << LEFT_BOTTOM_EDGE;
    for (int i(0); i < RowCount - 1; i++)
    {
        RenderHorizontalDelimitersPerCell();
        std::cout << BOTTOM_CENTER_PIECE;
    }

    RenderHorizontalDelimitersPerCell();
    std::cout << RIGHT_BOTTOM_EDGE << std::endl;
}

void TerminalView::RenderCell(const CellIndex&, const CellState InState)
{
    switch (InState)
    {
    case CellState::Concealed:
    {
        RenderSymbolNTimes(' ', HorizontalSymbolsAmountPerCell);
        break;
    }
    case CellState::Damaged:
    {
        // HACK for now, should be fixed with formatting approach
        if (HorizontalSymbolsAmountPerCell == 1)
        {
            std::cout << "x";
        }
        else if (HorizontalSymbolsAmountPerCell == 3)
        {
            std::cout << " x ";
        }
        break;
    }
    case CellState::Destroyed:
    {
        // HACK for now, should be fixed with formatting approach
        if (HorizontalSymbolsAmountPerCell == 1)
        {
            std::cout << "X";
        }
        else if (HorizontalSymbolsAmountPerCell == 3)
        {
            std::cout << " X ";
        }
        break;
    }
    case CellState::Missed:
    {
        RenderSymbolNTimes(EMPTY_FILLER, HorizontalSymbolsAmountPerCell);
        break;
    }
    default:
        break;
    }
}

void TerminalView::RenderHorizontalDelimitersPerCell()
{
    RenderSymbolNTimes(HORIZONTAL_PIECE, HorizontalSymbolsAmountPerCell);
}

void TerminalView::RenderVerticalDelimitersPerCell()
{
    RenderSymbolNTimes(VERTICAL_PIECE, VerticalSymbolsAmountPerCell);
}

void TerminalView::RenderLetterAxisWithAlignment(const char InSymbol)
{
    std::cout << " " << InSymbol << " ";
}

void TerminalView::RenderSymbolNTimes(const char InSymbol, const unsigned int InTimes)
{
    for (size_t i = 0; i < InTimes; i++)
    {
        std::cout << InSymbol;
    }
}
