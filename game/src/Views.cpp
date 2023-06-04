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
constexpr static uint8_t SpacesBetweenGrids = 10;

// TODO probably make sense to take the data from model when model is implemented.
// This array might be generated in compile time automatically with N chars starting from 'A' symbol
constexpr static std::array<char, 10> RowAxisNames =
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'
};


TerminalView::TerminalView(const std::shared_ptr<IBattleSeaGame>& InGame)
    : Game(InGame)
{
}

void TerminalView::RenderGame()
{
    // README for now Player_1 is user, Player_2 is bot
    // TODO Multiplayer requires understanding which EPlayer value current player has
    // Important: Own grid must visualize ships as well, opponent's - no.
    const GridData modelData_1 = Game->GetPlayerGridInfo(EPlayer::Player_1);
    const GridData modelData_2 = Game->GetPlayerGridInfo(EPlayer::Player_2);
    RenderTwoGrids(modelData_1, modelData_2, true);
}

// TODO add ships
void TerminalView::RenderSingleGrid(const GridData& InGridData)
{
    assert(InGridData.size() != 0 && InGridData.front().size() != 0);
    assert(InGridData.size() <= RowAxisNames.size());

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

// TODO render own ships
void TerminalView::RenderTwoGrids(const GridData& InGridDataLeft, const GridData& InGridDataRight, const bool bIsHorizontally/* = true*/)
{
    if (!bIsHorizontally)
    {
        RenderSingleGrid(InGridDataLeft);
        std::cout << std::endl;
        RenderSingleGrid(InGridDataRight);
        std::cout << std::endl;

        return;
    }

    // Check non-emptiness
    assert(InGridDataLeft.size() != 0 && InGridDataLeft.front().size() != 0);
    assert(InGridDataRight.size() != 0 && InGridDataRight.front().size() != 0);
    // Check size equality
    assert(InGridDataLeft.size() == InGridDataRight.size());
    assert(InGridDataLeft.front().size() == InGridDataRight.front().size());
    // Check that each row has own name
    assert(InGridDataLeft.size() <= RowAxisNames.size());

    const size_t RowCount = InGridDataLeft.size();
    const size_t ColCount = InGridDataLeft[0].size();

    const std::array grids = { InGridDataLeft, InGridDataRight };

    // Numbers top axis
    for (int count(0); count < grids.size(); count++)
    {
        RenderLetterAxisWithAlignment(' ');
        for (int i(1); i <= ColCount; i++)
        {
            // TODO improve this to be dependent on some formatting approach
            std::cout << "  " << i << " ";
        }
        RenderGridOffset();
    }
    std::cout << std::endl;

    // Grid top title
    for (int count(0); count < grids.size(); count++)
    {
        RenderLetterAxisWithAlignment(' ');
        std::cout << LEFT_TOP_EDGE;
        for (int i(0); i < ColCount - 1; i++)
        {
            RenderHorizontalDelimitersPerCell();
            std::cout << TOP_CENTER_PIECE;
        }

        RenderHorizontalDelimitersPerCell();
        std::cout << RIGHT_TOP_EDGE;
        RenderGridOffset();
    }
    std::cout << std::endl;

    // N - 1 lines of vertical delimiters and horizontal grid lines
    for (int i(0); i < RowCount - 1; i++)
    {
        for (int count(0); count < grids.size(); count++)
        {
            const GridData& grid = grids[count];
            RenderLetterAxisWithAlignment(RowAxisNames[i]);
            for (int j(0); j < ColCount; j++)
            {
                RenderVerticalDelimitersPerCell();
                const CellIndex Index = CellIndex(i, j);
                RenderCell(Index, grid[i][j]);
            }
            RenderVerticalDelimitersPerCell();

            RenderGridOffset();
        }
        std::cout << std::endl;

        for (int count(0); count < grids.size(); count++)
        {
            RenderLetterAxisWithAlignment(' ');
            std::cout << LEFT_CENTER_PIECE;
            for (int j(0); j < ColCount - 1; j++)
            {
                RenderHorizontalDelimitersPerCell();
                std::cout << CENTER_PIECE;
            }
            RenderHorizontalDelimitersPerCell();
            std::cout << RIGHT_CENTER_PIECE;

            RenderGridOffset();
        }
        std::cout << std::endl;
    }

    // Last layer of vertical delimiters
    for (int count(0); count < grids.size(); count++)
    {
        RenderLetterAxisWithAlignment(RowAxisNames.back());
        for (int i(0); i < ColCount; i++)
        {
            RenderVerticalDelimitersPerCell();
            const CellIndex Index = CellIndex(ColCount - 1, i);
            RenderCell(Index, grids[count].back()[i]);
        }

        RenderVerticalDelimitersPerCell();
        RenderGridOffset();
    }
    std::cout << std::endl;

    // Grid bottom title
    for (int count(0); count < grids.size(); count++)
    {
        RenderLetterAxisWithAlignment(' ');
        std::cout << LEFT_BOTTOM_EDGE;
        for (int i(0); i < ColCount - 1; i++)
        {
            RenderHorizontalDelimitersPerCell();
            std::cout << BOTTOM_CENTER_PIECE;
        }

        RenderHorizontalDelimitersPerCell();
        std::cout << RIGHT_BOTTOM_EDGE;

        RenderGridOffset();
    }
    std::cout << std::endl;
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

void TerminalView::RenderGridOffset()
{
    RenderSymbolNTimes(' ', SpacesBetweenGrids);
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
