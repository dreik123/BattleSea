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
    constexpr unsigned char SHIP_FILLER = 219;
}


constexpr static uint8_t HORIZONTAL_SYMBOLS_AMOUNT_PER_CELL = 3;
constexpr static uint8_t VERTICAL_SYMBOLS_AMOUNT_PER_CELL = 1;
constexpr static uint8_t SPACES_BETWEEN_GRIDS = 10;

// TODO probably make sense to take the data from model when model is implemented.
// This array might be generated in compile time automatically with N chars starting from 'A' symbol
constexpr static std::array<char, 10> ROW_AXIS_NAMES =
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'
};


TerminalView::TerminalView(const std::shared_ptr<IBattleSeaGame>& game)
    : m_game(game)
{
}

void TerminalView::renderGame()
{
    // README for now Player1 is user, Player2 is bot
    // TODO Multiplayer requires understanding which Player value current player has
    // Important: Own grid must visualize ships as well, opponent's - no.
    const GridData modelData1 = m_game->getPlayerGridInfo(Player::Player1);
    const GridData modelData2 = m_game->getPlayerGridInfo(Player::Player2);
    renderTwoGrids(modelData1, modelData2, true);
}

// TODO add ships
void TerminalView::renderSingleGrid(const GridData& gridData)
{
    assert(gridData.size() != 0 && gridData.front().size() != 0);
    assert(gridData.size() <= ROW_AXIS_NAMES.size());

    const size_t rowCount = gridData.size();
    const size_t colCount = gridData[0].size();

    renderLetterAxisWithAlignment(' ');
    for (int i(1); i <= colCount; i++)
    {
        // TODO improve this to be dependent on some formatting approach
        std::cout << "  " << i << " ";
    }
    std::cout << std::endl;

    renderLetterAxisWithAlignment(' ');
    std::cout << LEFT_TOP_EDGE;
    for (int i(0); i < rowCount - 1; i++)
    {
        renderHorizontalDelimitersPerCell();
        std::cout << TOP_CENTER_PIECE;
    }

    renderHorizontalDelimitersPerCell();
    std::cout << RIGHT_TOP_EDGE << std::endl;

    for (int i(0); i < colCount - 1; i++)
    {
        renderLetterAxisWithAlignment(ROW_AXIS_NAMES[i]);
        for (int j(0); j < rowCount; j++)
        {
            renderVerticalDelimitersPerCell();
            const CellIndex Index = CellIndex(i, j);
            renderCell(Index, gridData[i][j]);
        }
        renderVerticalDelimitersPerCell();
        std::cout << std::endl;

        renderLetterAxisWithAlignment(' ');
        std::cout << LEFT_CENTER_PIECE;
        for (int j(0); j < rowCount - 1; j++)
        {
            renderHorizontalDelimitersPerCell();
            std::cout << CENTER_PIECE;
        }
        renderHorizontalDelimitersPerCell();
        std::cout << RIGHT_CENTER_PIECE << std::endl;
    }

    renderLetterAxisWithAlignment(ROW_AXIS_NAMES.back());
    for (int i(0); i < rowCount; i++)
    {
        renderVerticalDelimitersPerCell();
        const CellIndex Index = CellIndex(rowCount - 1, i);
        renderCell(Index, gridData.back()[i]);
    }

    renderVerticalDelimitersPerCell();
    std::cout << std::endl;

    renderLetterAxisWithAlignment(' ');
    std::cout << LEFT_BOTTOM_EDGE;
    for (int i(0); i < rowCount - 1; i++)
    {
        renderHorizontalDelimitersPerCell();
        std::cout << BOTTOM_CENTER_PIECE;
    }

    renderHorizontalDelimitersPerCell();
    std::cout << RIGHT_BOTTOM_EDGE << std::endl;
}

// TODO render own ships
void TerminalView::renderTwoGrids(const GridData& gridDataLeft, const GridData& gridDataRight, const bool isHorizontally/* = true*/)
{
    if (!isHorizontally)
    {
        renderSingleGrid(gridDataLeft);
        std::cout << std::endl;
        renderSingleGrid(gridDataRight);
        std::cout << std::endl;

        return;
    }

    // Check non-emptiness
    assert(gridDataLeft.size() != 0 && gridDataLeft.front().size() != 0);
    assert(gridDataRight.size() != 0 && gridDataRight.front().size() != 0);
    // Check size equality
    assert(gridDataLeft.size() == gridDataRight.size());
    assert(gridDataLeft.front().size() == gridDataRight.front().size());
    // Check that each row has own name
    assert(gridDataLeft.size() <= ROW_AXIS_NAMES.size());

    const size_t rowCount = gridDataLeft.size();
    const size_t colCount = gridDataLeft[0].size();

    const std::array grids = { gridDataLeft, gridDataRight };

    // Numbers top axis
    for (int count(0); count < grids.size(); count++)
    {
        renderLetterAxisWithAlignment(' ');
        for (int i(1); i <= colCount; i++)
        {
            // TODO improve this to be dependent on some formatting approach
            std::cout << "  " << i << " ";
        }
        renderGridOffset();
    }
    std::cout << std::endl;

    // Grid top title
    for (int count(0); count < grids.size(); count++)
    {
        renderLetterAxisWithAlignment(' ');
        std::cout << LEFT_TOP_EDGE;
        for (int i(0); i < colCount - 1; i++)
        {
            renderHorizontalDelimitersPerCell();
            std::cout << TOP_CENTER_PIECE;
        }

        renderHorizontalDelimitersPerCell();
        std::cout << RIGHT_TOP_EDGE;
        renderGridOffset();
    }
    std::cout << std::endl;

    // N - 1 lines of vertical delimiters and horizontal grid lines
    for (int i(0); i < rowCount - 1; i++)
    {
        for (int count(0); count < grids.size(); count++)
        {
            const GridData& grid = grids[count];
            renderLetterAxisWithAlignment(ROW_AXIS_NAMES[i]);
            for (int j(0); j < colCount; j++)
            {
                renderVerticalDelimitersPerCell();
                const CellIndex Index = CellIndex(i, j);
                renderCell(Index, grid[i][j]);
            }
            renderVerticalDelimitersPerCell();

            renderGridOffset();
        }
        std::cout << std::endl;

        for (int count(0); count < grids.size(); count++)
        {
            renderLetterAxisWithAlignment(' ');
            std::cout << LEFT_CENTER_PIECE;
            for (int j(0); j < colCount - 1; j++)
            {
                renderHorizontalDelimitersPerCell();
                std::cout << CENTER_PIECE;
            }
            renderHorizontalDelimitersPerCell();
            std::cout << RIGHT_CENTER_PIECE;

            renderGridOffset();
        }
        std::cout << std::endl;
    }

    // Last layer of vertical delimiters
    for (int count(0); count < grids.size(); count++)
    {
        renderLetterAxisWithAlignment(ROW_AXIS_NAMES.back());
        for (int i(0); i < colCount; i++)
        {
            renderVerticalDelimitersPerCell();
            const CellIndex Index = CellIndex(colCount - 1, i);
            renderCell(Index, grids[count].back()[i]);
        }

        renderVerticalDelimitersPerCell();
        renderGridOffset();
    }
    std::cout << std::endl;

    // Grid bottom title
    for (int count(0); count < grids.size(); count++)
    {
        renderLetterAxisWithAlignment(' ');
        std::cout << LEFT_BOTTOM_EDGE;
        for (int i(0); i < colCount - 1; i++)
        {
            renderHorizontalDelimitersPerCell();
            std::cout << BOTTOM_CENTER_PIECE;
        }

        renderHorizontalDelimitersPerCell();
        std::cout << RIGHT_BOTTOM_EDGE;

        renderGridOffset();
    }
    std::cout << std::endl;
}

void TerminalView::renderCell(const CellIndex&, const CellState state)
{
    switch (state)
    {
    case CellState::Concealed:
    {
        renderSymbolNTimes(' ', HORIZONTAL_SYMBOLS_AMOUNT_PER_CELL);
        break;
    }
    case CellState::Damaged:
    {
        // HACK for now, should be fixed with formatting approach
        if (HORIZONTAL_SYMBOLS_AMOUNT_PER_CELL == 1)
        {
            std::cout << "x";
        }
        else if (HORIZONTAL_SYMBOLS_AMOUNT_PER_CELL == 3)
        {
            std::cout << " x ";
        }
        break;
    }
    case CellState::Destroyed:
    {
        // HACK for now, should be fixed with formatting approach
        if (HORIZONTAL_SYMBOLS_AMOUNT_PER_CELL == 1)
        {
            std::cout << "X";
        }
        else if (HORIZONTAL_SYMBOLS_AMOUNT_PER_CELL == 3)
        {
            std::cout << " X ";
        }
        break;
    }
    case CellState::Missed:
    {
        renderSymbolNTimes(EMPTY_FILLER, HORIZONTAL_SYMBOLS_AMOUNT_PER_CELL);
        break;
    }
    case CellState::Ship:
    {
        renderSymbolNTimes(SHIP_FILLER, HORIZONTAL_SYMBOLS_AMOUNT_PER_CELL);
        break;
    }
    default:
        break;
    }
}

void TerminalView::renderHorizontalDelimitersPerCell()
{
    renderSymbolNTimes(HORIZONTAL_PIECE, HORIZONTAL_SYMBOLS_AMOUNT_PER_CELL);
}

void TerminalView::renderVerticalDelimitersPerCell()
{
    renderSymbolNTimes(VERTICAL_PIECE, VERTICAL_SYMBOLS_AMOUNT_PER_CELL);
}

void TerminalView::renderGridOffset()
{
    renderSymbolNTimes(' ', SPACES_BETWEEN_GRIDS);
}

void TerminalView::renderLetterAxisWithAlignment(const char symbol)
{
    std::cout << " " << symbol << " ";
}

void TerminalView::renderSymbolNTimes(const char symbol, const unsigned int times)
{
    for (size_t i = 0; i < times; i++)
    {
        std::cout << symbol;
    }
}
