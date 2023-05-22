#include "WarShipGenerators.h"

const std::vector<WarShip> PredefinedClassicWarShipGenerator::GenerateShips(const GameConfig&)
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
    assert(allShipCells[0][0].ToString(false) == "a2");
    assert(allShipCells[7][0].ToString(true) == "D10");
    const auto& [x, y] = allShipCells[9][0].AsIndexesPair();
    assert(x == 7 && y == 6);

    std::vector<WarShip> result;
    for (const auto& cells : allShipCells)
    {
        result.emplace_back(cells);
    }
    return result;
}

const std::vector<WarShip> WarShipGenerator::GenerateShips(const GameConfig& params)
{
    // TODO AP real version
    return std::vector<WarShip>();
}
