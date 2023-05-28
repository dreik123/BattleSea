#include "Views.h"
#include "GameInterfaces.h"         // for IBattleSeaGame

#include <iostream>  // is temporary used for console presentation


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

    return gridData;
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
        for (int j(0); j < RowCount; j++)
        {
            RenderVerticalDelimitersPerCell();
            const CellIndex Index = CellIndex(i, j);
            RenderCell(Index, InGridData[i][j]);
        }
        RenderVerticalDelimitersPerCell();
        std::cout << std::endl;

        std::cout << LEFT_CENTER_PIECE;
        for (int j(0); j < RowCount - 1; j++)
        {
            RenderHorizontalDelimitersPerCell();
            std::cout << CENTER_PIECE;
        }
        RenderHorizontalDelimitersPerCell();
        std::cout << RIGHT_CENTER_PIECE << std::endl;
    }

    for (int i(0); i < RowCount; i++)
    {
        RenderVerticalDelimitersPerCell();
        const CellIndex Index = CellIndex(RowCount - 1, i);
        RenderCell(Index, InGridData[RowCount - 1][i]);
    }

    RenderVerticalDelimitersPerCell();
    std::cout << std::endl;

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
            std::cout << "   ";
            break;
        }
        case CellState::Damaged:
        {
            std::cout << " x ";
            break;
        }
        case CellState::Destroyed:
        {
            std::cout << " X ";
            break;
        }
        case CellState::Missed:
        {
            std::cout << EMPTY_FILLER << EMPTY_FILLER << EMPTY_FILLER;
            break;
        }
    default:
        break;
    }
}

void TerminalView::RenderHorizontalDelimitersPerCell()
{
    for (size_t i = 0; i < HorizontalSymbolsAmountPerCell; i++)
    {
        std::cout << HORIZONTAL_PIECE;
    }
}

void TerminalView::RenderVerticalDelimitersPerCell()
{
    for (size_t i = 0; i < VerticalSymbolsAmountPerCell; i++)
    {
        std::cout << VERTICAL_PIECE;
    }
}
