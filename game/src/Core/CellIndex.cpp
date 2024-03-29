#include "CellIndex.h"
#include "CharUtilities.h"

CellIndex::CellIndex(int x, int y)
    : m_internalCoordinates{ static_cast<int8_t>(x), static_cast<int8_t>(y) }
{
}

CellIndex CellIndex::fromString(const std::string& coord)
{
    assert(
        (coord.size() == 2 && isalpha(coord[0]) && isdigit(coord[1])) ||
        (coord.size() == 3 && isalpha(coord[0]) && isdigit(coord[1]) && isdigit(coord[2]))
    );

    int x = char_utilities::letterCharToInt(coord[0]);
    int y = 0;
    if (coord.size() == 2)
    {
        y = char_utilities::digitCharToInt(coord[1]) - 1; // -1 to handle array index and view difference
    }
    else if (coord.size() == 3)
    {
        y = std::stoi(coord.substr(1, 2)) - 1; // -1 to handle array index and view difference
    }

    // Let controller outside to validate input
    return CellIndex{ x, y };
}

const std::string CellIndex::toString(const bool isUpperCase) const
{
    std::string result;
    result.push_back(char_utilities::indexToLetterChar(m_internalCoordinates.first, isUpperCase));
    result += char_utilities::indexToDigitStr(m_internalCoordinates.second);

    return result;
}


// TODO consider separate place and approach if need for these tests
constexpr void cellIndexTests()
{
    using namespace char_utilities;

    static_assert(letterCharToInt('a') == 0);
    static_assert(letterCharToInt('b') == 1);
    static_assert(charToLower('C') == 'c');
    static_assert(letterCharToInt(charToLower('C')) == 2);
    static_assert(letterCharToInt('C') == 2);
    static_assert(letterCharToInt('c') == 2);

    static_assert(indexToLetterChar(4, false) == 'e');
    static_assert(indexToLetterChar(5, true) == 'F');
    //static_assert(IndexToDigitStr(1) == "2");
    //static_assert(IndexToDigitStr(9) == "10");
}
