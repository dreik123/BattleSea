#include "WarShipGenerators.h"
#include <algorithm>
#include <iostream> //DEBUG
#include <random>


const std::vector<WarShip> PredefinedClassicWarShipGenerator::generateShips(const GameConfig&)
{
    /*
      __________
    A| X    X   |
    B|          |
    C|  X   X   |
    D|         X|
    E|X  X  XX X|
    F|X  X     X|
    G|          |
    H|  X   XXXX|
    I|  X       |
    J|  X       |
      12345678910
    */

    std::array<std::vector<CellIndex>, 10> allShipCells;
    allShipCells[0] = { CellIndex{"a2"} };
    allShipCells[1] = { CellIndex{"a7"} };
    allShipCells[2] = { CellIndex{"c3"} };
    allShipCells[3] = { CellIndex{"c7"} };
    allShipCells[4] = { CellIndex{"e1"}, CellIndex{"f1"} };
    allShipCells[5] = { CellIndex{"e4"}, CellIndex{"f4"} };
    allShipCells[6] = { CellIndex{"e7"}, CellIndex{"e8"} };
    allShipCells[7] = { CellIndex{"d10"}, CellIndex{"e10"}, CellIndex{"f10"} };
    allShipCells[8] = { CellIndex{"h3"}, CellIndex{"i3"}, CellIndex{"j3"} };
    allShipCells[9] = { CellIndex{"h7"}, CellIndex{"h8"}, CellIndex{"h9"}, CellIndex{"h10"} };

    // Some tests (TODO DS move to separate place)
    assert(allShipCells[0][0].toString(false) == "a2");
    assert(allShipCells[7][0].toString(true) == "D10");
    const auto& [x, y] = allShipCells[9][0].asIndexesPair();
    assert(x == 7 && y == 6);

    std::vector<WarShip> result;
    for (const auto& cells : allShipCells)
    {
        result.emplace_back(cells);
    }
    return result;
}


bool WarShipGenerator::setShipCell(
    const CellIndex& coord,
    const Direction& direction,
    std::vector<Direction>& permissionDirections,
    const std::vector<std::vector<int>>& cells,
    std::vector<CellIndex>& shipCells)
{
    if (cells[coord.x()][coord.y()] != 0)
    {
        permissionDirections.erase(std::remove(permissionDirections.begin(), permissionDirections.end(), direction), permissionDirections.end());
        return false;
    }

    shipCells.push_back(coord);
    return true;
}

void WarShipGenerator::fillAreaAroundShip(const std::vector<CellIndex>& shipCells, std::vector<std::vector<int>>& cells, const GameConfig& params)
{
    for (auto& cell : shipCells)
    {
        std::vector<CellIndex> tempCells{
        CellIndex(cell.x(), cell.y() - 1),
        CellIndex(cell.x() + 1, cell.y()),
        CellIndex(cell.x(), cell.y() + 1),
        CellIndex(cell.x() - 1, cell.y()),
        CellIndex(cell.x() + 1, cell.y() - 1),
        CellIndex(cell.x() - 1, cell.y() - 1),
        CellIndex(cell.x() + 1, cell.y() + 1),
        CellIndex(cell.x() - 1, cell.y() + 1) };

        for (auto& c : tempCells)
        {
            if (c.x() >= 0 && c.x() < params.m_sizeX &&
                c.y() >= 0 && c.y() < params.m_sizeY &&
                cells[c.x()][c.y()] == 0)
            {
                cells[c.x()][c.y()] = 2;
            }
        }
    }
}

const std::vector<WarShip> WarShipGenerator::generateShips(const GameConfig& params)
{
    std::vector<std::vector<int>> cells;

    for (int i = 0; i < params.m_sizeX; ++i)
    {
        std::vector<int> temp;
        for (int j = 0; j < params.m_sizeY; ++j)
        {
            temp.push_back(0);
        }
        cells.push_back(temp);
    }

    std::vector<WarShip> resWarships;
    for (auto shipIt = params.m_amountOfShip.crbegin(); shipIt != params.m_amountOfShip.crend(); ++shipIt)
    {
        std::vector<CellIndex> shipCells;

        const int amountOfShipDecks = static_cast<int>(*shipIt);
        std::random_device rd;
        std::mt19937 mt(rd());

        bool isValid = true;
        std::uniform_int_distribution<int> distX(0, params.m_sizeX - 1);
        std::uniform_int_distribution<int> distY(0, params.m_sizeY - 1);

        const CellIndex firstCell(distX(mt), distY(mt));

        if (cells[firstCell.x()][firstCell.y()] != 0)
        {
            --shipIt;
            continue;
        }
        std::vector<Direction> permissionDirections{ Direction::NORTH,
                                                     Direction::EAST,
                                                     Direction::SOUTH,
                                                     Direction::WEST };

        if (firstCell.y() - amountOfShipDecks - 1 < 0)
        {
            permissionDirections.erase(std::remove(permissionDirections.begin(), permissionDirections.end(), Direction::NORTH), permissionDirections.end());
        }
        if (firstCell.x() + amountOfShipDecks - 1 >= params.m_sizeX)
        {
            permissionDirections.erase(std::remove(permissionDirections.begin(), permissionDirections.end(), Direction::EAST), permissionDirections.end());
        }
        if (firstCell.y() + amountOfShipDecks - 1 >= params.m_sizeY)
        {
            permissionDirections.erase(std::remove(permissionDirections.begin(), permissionDirections.end(), Direction::SOUTH), permissionDirections.end());
        }
        if (firstCell.x() - amountOfShipDecks - 1 < 0)
        {
            permissionDirections.erase(std::remove(permissionDirections.begin(), permissionDirections.end(), Direction::WEST), permissionDirections.end());
        }

        if (permissionDirections.empty())
        {
            --shipIt;
            continue;
        }

        do
        {
            if (permissionDirections.empty())
            {
                break;
            }
            isValid = true;

            std::uniform_int_distribution<int> directionRand(0, permissionDirections.size() - 1);
            Direction direction = permissionDirections.at(directionRand(mt));

            for (int i = 1; i < amountOfShipDecks; ++i)
            {
                if (!isValid)
                {
                    break;
                }
                switch (direction)
                {
                case Direction::NORTH:
                    isValid = setShipCell(CellIndex(firstCell.x(), firstCell.y() - i), direction, permissionDirections, cells, shipCells);
                    break;
                case Direction::EAST:
                    isValid = setShipCell(CellIndex(firstCell.x() + i, firstCell.y()), direction, permissionDirections, cells, shipCells);
                    break;
                case Direction::SOUTH:
                    isValid = setShipCell(CellIndex(firstCell.x(), firstCell.y() + i), direction, permissionDirections, cells, shipCells);
                    break;
                case Direction::WEST:
                    isValid = setShipCell(CellIndex(firstCell.x() - i, firstCell.y()), direction, permissionDirections, cells, shipCells);
                    break;
                default:
                    break;
                }
            }
            if (!isValid)
            {
                continue;
            }
        } while (!isValid);

        if (permissionDirections.empty())
        {
            --shipIt;
            continue;
        }
        cells[firstCell.x()][firstCell.y()] = 1;
        shipCells.emplace_back(firstCell);

        for (auto& c : shipCells)
        {
            cells[c.x()][c.y()] = 1;
        }
        fillAreaAroundShip(shipCells, cells, params);

        resWarships.push_back(WarShip(shipCells));
    }

    return resWarships;
}
