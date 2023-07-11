#include "Views.h"

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
    constexpr unsigned char SHIP_FILLER = 219;
}

constexpr static uint8_t HORIZONTAL_SYMBOLS_AMOUNT_PER_CELL = 3;
constexpr static uint8_t VERTICAL_SYMBOLS_AMOUNT_PER_CELL = 1;
constexpr static uint8_t SPACES_BETWEEN_GRIDS = 10;


TerminalView::TerminalView(std::shared_ptr<EventBus>& bus)
    : m_eventBus(bus)
{
}

void TerminalView::renderStartScreen()
{
    std::cout << "Welcome to Battle Sea game. Enjoy it.\nPress any key to choose your setup\n";
}

void TerminalView::renderGameGrids(const GameGrid modelData1, const GameGrid modelData2)
{
    renderTwoGrids(modelData1, modelData2, true);
}

void TerminalView::renderShotError(const ShotError error)
{
    switch (error)
    {
    case ShotError::Ok:
        break;
    case ShotError::OutOfGrid:
        std::cout << "Out of grid. Try again\n";
        break;
    case ShotError::RepeatedShot:
        std::cout << "You've already shooted at this cell! Try again\n";
        break;
    default:
        assert(false && "Unexpected ShotError. Please process it!");
        break;
    }
}

void TerminalView::renderGameOver(const std::string winnerName, const bool isLocalPlayer)
{
    std::cout << winnerName << (isLocalPlayer ? "(You)" : "") << " won!\n";
}

void TerminalView::renderGeneratedShips(const GameGrid& grid)
{
    system("cls");
    std::cout << "Generated the ships for you:\n";
    renderSingleGrid(grid);
}

// TODO add ships
void TerminalView::renderSingleGrid(const GameGrid& grid)
{
    assert(grid.data.size() != 0 && grid.data.front().size() != 0);
    assert(grid.data.size() <= ROW_AXIS_NAMES.size());

    const size_t rowCount = grid.data.size();
    const size_t colCount = grid[0].size();

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
            const CellIndex index = CellIndex(i, j);
            renderCell(index, grid.at(index));
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
        const CellIndex index = CellIndex(rowCount - 1, i);
        renderCell(index, grid.at(index));
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

void TerminalView::renderTwoGrids(const GameGrid& gridLeft, const GameGrid& gridRight, const bool isHorizontally/* = true*/)
{
    if (!isHorizontally)
    {
        renderSingleGrid(gridLeft);
        std::cout << std::endl;
        renderSingleGrid(gridRight);
        std::cout << std::endl;

        return;
    }

    // Check non-emptiness
    assert(gridLeft.data.size() != 0 && gridLeft.data.front().size() != 0);
    assert(gridRight.data.size() != 0 && gridRight.data.front().size() != 0);
    // Check size equality
    assert(gridLeft.data.size() == gridRight.data.size());
    assert(gridLeft.data.front().size() == gridRight.data.front().size());
    // Check that each row has own name
    assert(gridLeft.data.size() <= ROW_AXIS_NAMES.size());

    const size_t rowCount = gridLeft.data.size();
    const size_t colCount = gridLeft[0].size();

    const std::array grids = { gridLeft, gridRight };

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
            const GameGrid& grid = grids[count];
            renderLetterAxisWithAlignment(ROW_AXIS_NAMES[i]);
            for (int j(0); j < colCount; j++)
            {
                renderVerticalDelimitersPerCell();
                const CellIndex index = CellIndex(i, j);
                renderCell(index, grid.at(index));
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
            const CellIndex index = CellIndex(colCount - 1, i);
            renderCell(index, grids[count].at(index));
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
