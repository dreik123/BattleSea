#include "Views.h"
#include "GameInterfaces.h"         // for IBattleSeaGame

#include <iostream>  // is temporary used for console presentation


// Constants
namespace 
{
    constexpr unsigned LEFT_TOP_EDGE = 201;
    constexpr unsigned RIGHT_TOP_EDGE = 187;
    constexpr unsigned LEFT_BOTTOM_EDGE = 200;
    constexpr unsigned RIGHT_BOTTOM_EDGE = 188;

    constexpr unsigned LEFT_CENTER_PIECE = 204;
    constexpr unsigned RIGHT_CENTER_PIECE = 185;
    constexpr unsigned TOP_CENTER_PIECE = 203;
    constexpr unsigned BOTTOM_CENTER_PIECE = 202;

    constexpr unsigned VERTICAL_PIECE = 186;
    constexpr unsigned HORIZONTAL_PIECE = 205;

    constexpr unsigned CENTER_PIECE = 206;

    constexpr unsigned EMPTY_FILLER = 176;

    // TODO refactor this ugly macro
#define HORIZ_SNIPPET (char)HORIZONTAL_PIECE << (char)HORIZONTAL_PIECE << (char)HORIZONTAL_PIECE
}


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
    // TODO data validation
    const size_t RowCount = InGridData.size();
    const size_t ColCount = InGridData[0].size();

    std::cout << (char)LEFT_TOP_EDGE;
    for (int i(0); i < RowCount - 1; i++)
    {
        std::cout << HORIZ_SNIPPET << (char)TOP_CENTER_PIECE;
    }
    std::cout << HORIZ_SNIPPET << (char)RIGHT_TOP_EDGE << std::endl;

    for (int i(0); i < ColCount - 1; i++)
    {
        for (int j(0); j < RowCount; j++)
        {
            std::cout << (char)VERTICAL_PIECE;
            const CellIndex Index = CellIndex(i, j);
            RenderCell(Index, InGridData[i][j]);
        }
        std::cout << (char)VERTICAL_PIECE << std::endl;

        std::cout << (char)LEFT_CENTER_PIECE;
        for (int j(0); j < RowCount - 1; j++)
        {
            std::cout << HORIZ_SNIPPET << (char)CENTER_PIECE;
        }
        std::cout << HORIZ_SNIPPET << (char)RIGHT_CENTER_PIECE << std::endl;
    }

    for (int i(0); i < RowCount; i++)
    {
        std::cout << (char)VERTICAL_PIECE;
        const CellIndex Index = CellIndex(RowCount - 1, i);
        RenderCell(Index, InGridData[RowCount - 1][i]);
    }
    std::cout << (char)VERTICAL_PIECE << std::endl;

    std::cout << (char)LEFT_BOTTOM_EDGE;
    for (int i(0); i < RowCount - 1; i++)
    {
        std::cout << HORIZ_SNIPPET << (char)BOTTOM_CENTER_PIECE;
    }
    std::cout << HORIZ_SNIPPET << (char)RIGHT_BOTTOM_EDGE << std::endl;
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
            std::cout << (char)EMPTY_FILLER << (char)EMPTY_FILLER << (char)EMPTY_FILLER;
            break;
        }
    default:
        break;
    }
}
